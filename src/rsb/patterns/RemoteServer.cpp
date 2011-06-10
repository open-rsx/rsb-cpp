/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "RemoteServer.h"

#include <stdexcept>

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include <rsc/misc/UUID.h>

#include "../Factory.h"
#include "../Handler.h"

using namespace std;

namespace rsb {
namespace patterns {

class WaitingEventHandler: public Handler {
private:

    rsc::logging::LoggerPtr logger;

    boost::mutex mutex;
    boost::condition condition;

    set<string> waitForEvents;
    map<string, EventPtr> storedEvents;

public:

    WaitingEventHandler(rsc::logging::LoggerPtr logger) :
        logger(logger) {
    }

    string getClassName() const {
        return "WaitingEventHandler";
    }

    void handle(EventPtr event) {
        {
            boost::mutex::scoped_lock lock(mutex);
            if (event
                    && event->mutableMetaData().hasUserInfo("ServerRequestId")
                    && waitForEvents.count(
                            event->mutableMetaData().getUserInfo(
                                    "ServerRequestId"))) {
                RSCDEBUG(logger, "Received reply event " << *event);
                waitForEvents.erase(event->mutableMetaData().getUserInfo(
                        "ServerRequestId"));
                storedEvents[event->mutableMetaData().getUserInfo(
                        "ServerRequestId")] = event;
            } else {
                RSCTRACE(logger, "Received uninteresting event " << *event);
            }
        }
        condition.notify_all();
    }

    void expectReply(const string &requestId) {
        boost::mutex::scoped_lock lock(mutex);
        waitForEvents.insert(requestId);
    }

    EventPtr getReply(const string &requestId) {

        RSCTRACE(logger, "Waiting for reply with id " << requestId);

        boost::mutex::scoped_lock lock(mutex);
        while (!storedEvents.count(requestId)) {
            boost::xtime xt;
            xtime_get(&xt, boost::TIME_UTC);
            // TODO make max time a configuration property
            xt.sec += 25;
            if (!condition.timed_wait(lock, xt)) {
                RSCERROR(logger, "Timeout while waiting");
                throw RemoteServer::TimeoutException(
                        "Error calling method and waiting for reply with id "
                                + requestId + ". Waited 25 seconds.");
            }
        }

        EventPtr reply = storedEvents[requestId];
        storedEvents.erase(requestId);

        RSCDEBUG(logger, "Received correct reply event " << *reply);
        return reply;
    }

};

RemoteServer::TimeoutException::TimeoutException(const string &message) :
    Exception(message) {
}

RemoteServer::RemoteTargetInvocationException::RemoteTargetInvocationException(
        const string &message) :
    Exception(message) {
}

RemoteServer::RemoteServer(const Scope &scope) :
    logger(rsc::logging::Logger::getLogger("rsc.patterns.RemoteServer."
            + scope.toString())), scope(scope) {
    // TODO check that this server is alive...
    // TODO probably it would be a good idea to request some method infos from
    //      the server, e.g. for type checking
}

RemoteServer::~RemoteServer() {
}

RemoteServer::MethodSet RemoteServer::getMethodSet(const string &methodName,
        const string &sendType) {

    boost::mutex::scoped_lock lock(methodSetMutex);

    if (!methodSets.count(methodName)) {

        // start a listener to wait for the reply
        const Scope replyScope = scope.concat(Scope("/reply")).concat(Scope("/"
                + methodName));
        ListenerPtr listener =
                Factory::getInstance().createListener(replyScope);

        boost::shared_ptr<WaitingEventHandler> handler(new WaitingEventHandler(
                logger));
        listener->addHandler(handler);

        // informer for requests
        Informer<void>::Ptr informer = Factory::getInstance().createInformer<
                void> (scope.concat(Scope("/request")).concat(Scope("/"
                + methodName)),
                Factory::getInstance().getDefaultParticipantConfig(), sendType);

        MethodSet set;
        set.methodName = methodName;
        set.sendType = sendType;
        set.handler = handler;
        set.replyListener = listener;
        set.requestInformer = informer;

        methodSets[methodName] = set;

    }

    if (methodSets[methodName].sendType != sendType) {
        throw runtime_error("Illegal send type. Method previously accepted "
                + methodSets[methodName].sendType + " but now " + sendType
                + " was requested");
    }

    return methodSets[methodName];

}

EventPtr RemoteServer::callMethod(const string &methodName, EventPtr data) {

    RSCDEBUG(logger, "Calling method " << methodName << " with data " << data);

    // TODO check that the desired method exists

    string requestId = rsc::misc::UUID().getIdAsString();
    // TODO duplicated string from Server
    data->mutableMetaData().setUserInfo("ServerRequestId", requestId);

    MethodSet methodSet = getMethodSet(methodName, data->getType());
    methodSet.handler->expectReply(requestId);

    data->setScope(methodSet.requestInformer->getScope());
    methodSet.requestInformer->publish(data);

    // wait for the reply
    EventPtr result = methodSet.handler->getReply(requestId);
    if (result->mutableMetaData().hasUserInfo("isException")) {
        assert(result->getType() == "string");
        throw RemoteTargetInvocationException("Error calling remote method '"
                + methodName + "': " + *(boost::static_pointer_cast<string>(
                result->getData())));
    } else {
        return result;
    }

}

}
}
