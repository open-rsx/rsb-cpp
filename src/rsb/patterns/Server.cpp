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

#include "Server.h"

#include <stdexcept>

#include <rsc/logging/Logger.h>

#include "../Factory.h"
#include "MethodExistsException.h"

using namespace std;

namespace rsb {
namespace patterns {

class RequestHandler: public Handler {
private:

    rsc::logging::LoggerPtr logger;

    string methodName;
    Server::CallbackPtr callback;
    Informer<void>::Ptr informer;

public:

    RequestHandler(const string &methodName, Server::CallbackPtr callback,
            Informer<void>::Ptr informer) :
        logger(rsc::logging::Logger::getLogger("rsb.patterns.RequestHandler."
                + methodName)), methodName(methodName), callback(callback),
                informer(informer) {
    }

    void handle(EventPtr event) {

        const string requestIdKey = "ServerRequestId";

        if (event->getType() != callback->getRequestType()) {
            RSCERROR(logger, "Request type '" << event->getType()
                    << "' does not match expected request type '"
                    << callback->getRequestType() << "' of method '"
                    << methodName << "'");
            return;
        }

        if (!event->hasMetaInfo(requestIdKey)) {
            RSCERROR(logger,
                    "Request event does not contain a valid request ID "
                    << "to answer to.");
            return;
        }

        try {
            VoidPtr returnData = callback->intlCall(methodName,
                    event->getData());
            EventPtr returnEvent(new Event());
            returnEvent->setType(callback->getReplyType());
            returnEvent->setData(returnData);
            returnEvent ->addMetaInfo(requestIdKey, event->getMetaInfo(
                    requestIdKey));
            informer->publish(returnEvent);
        } catch (exception &e) {
            EventPtr returnEvent(new Event());
            returnEvent->setType("string");
            string exceptionType = typeid(e).name();
            returnEvent->setData(boost::shared_ptr<string>(new string(
                    exceptionType + ": " + e.what())));
            returnEvent->addMetaInfo(requestIdKey, event->getMetaInfo(
                    requestIdKey));
            returnEvent->addMetaInfo("isException", "");
            informer->publish(returnEvent);
        }

    }

};

Server::Server(const Scope &scope) :
    scope(scope) {
}

Server::~Server() {
}

void Server::registerMethod(const std::string &methodName, CallbackPtr callback) {

    // check that method does not exist
    if (methods.count(methodName)) {
        throw MethodExistsException(methodName, scope.toString());
    }

    // TODO check that the reply type is convertible
    Informer<void>::Ptr informer =
            Factory::getInstance().createInformer<void> (scope.concat(Scope(
                    "reply")).concat(Scope(methodName)),
                    Factory::getInstance().getDefaultParticipantConfig(),
                    callback->getReplyType());

    ListenerPtr listener = Factory::getInstance().createListener(scope.concat(
            Scope("request")).concat(Scope(methodName)));
    listener->addHandler(HandlerPtr(new RequestHandler(methodName, callback,
            informer)));
    this->requestListeners.insert(listener);

    methods[methodName] = informer;

}

}
}
