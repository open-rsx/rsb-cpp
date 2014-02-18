/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 *               2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
 *
 * ============================================================ */

#include "RemoteServer.h"

#include <stdexcept>

#include <boost/format.hpp>
#include <boost/thread/mutex.hpp>

#include <rsc/runtime/TypeStringTools.h>
#include <rsc/misc/UUID.h>

#include "../Factory.h"
#include "../Handler.h"
#include "../MetaData.h"
#include "../EventId.h"

using namespace std;

using namespace boost;

using namespace rsc::runtime;
using namespace rsc::logging;
using namespace rsc::threading;

namespace rsb {
namespace patterns {

/**
 * @author jwienke
 */
class WaitingEventHandler: public Handler {
public:
    typedef boost::recursive_mutex MutexType;
private:
    LoggerPtr logger;

    MutexType mutex;

    class RequestData {
    public:
        RequestData() {
            // for map use
        }
        RequestData(RemoteServer::FuturePtr future,
                ::rsb::HandlerPtr intermediateHandler) :
                future(future), intermediateHandler(intermediateHandler) {
        }
        RemoteServer::FuturePtr future;
        ::rsb::HandlerPtr intermediateHandler;
    };

    map<EventId, RequestData> inprogress;
public:

    WaitingEventHandler(LoggerPtr logger) :
        logger(logger) {
    }

    string getClassName() const {
        return "WaitingEventHandler";
    }

    MutexType& getMutex() {
        return this->mutex;
    }

    void handle(EventPtr event) {
        if (!event
            || event->getCauses().empty()) {
            RSCTRACE(logger, "Received an empty pointer or event without causes");
            return;
        }

        EventId requestId = *event->getCauses().begin();
        {
            MutexType::scoped_lock lock(mutex);

            if (!this->inprogress.count(requestId)) {
                RSCTRACE(logger, "Received uninteresting event " << event);
                return;
            }

            if (event->getMethod() == "REPLY") {
                handleReply(requestId, event);
            } else if (event->getMethod().empty()) {
                // anything without method must be an intermediate event
                HandlerPtr intermediateHandler =
                        this->inprogress[requestId].intermediateHandler;
                if (intermediateHandler) {
                    intermediateHandler->handle(event);
                }
            } else {
                RSCTRACE(logger, "Don't know what to do with event " << event);
            }

        }
    }

    void addCall(const EventId& requestId, RemoteServer::FuturePtr result,
            ::rsb::HandlerPtr intermediateHandler) {
        RSCTRACE(logger,
                "From now on handling a method call with request event ID "
                        << requestId);
        MutexType::scoped_lock lock(this->mutex);
        this->inprogress.insert(
                make_pair(requestId, RequestData(result, intermediateHandler)));
    }

private:

    void handleReply(const EventId& requestId, EventPtr event) {
        RSCDEBUG(logger, "Received reply event " << event);

        RemoteServer::FuturePtr result = this->inprogress[requestId].future;
        if (event->mutableMetaData().hasUserInfo("rsb:error?")) {
            assert(event->getType() == typeName<string>());
            result->setError(
                    str(
                            format("Error calling remote method '%1%': %2%")
                                    % "TODO: obtain method name"
                                    % *(boost::static_pointer_cast<string>(
                                            event->getData()))));
        } else {
            result->set(event);
        }
        this->inprogress.erase(requestId);
    }

};

RemoteServer::RemoteServer(const Scope& scope,
        const ParticipantConfig &listenerConfig,
        const ParticipantConfig &informerConfig) :
        logger(
                Logger::getLogger(
                        str(
                                format("rsb.patterns.RemoteServer[%1%]")
                                        % scope.toString()))), scope(scope), listenerConfig(
                listenerConfig), informerConfig(informerConfig) {
    // TODO check that this server is alive...
    // TODO probably it would be a good idea to request some method infos from
    //      the server, e.g. for type checking
}

RemoteServer::~RemoteServer() {
}

RemoteServer::MethodSet RemoteServer::getMethodSet(const string& methodName,
        const string& sendType) {

    boost::mutex::scoped_lock lock(methodSetMutex);

    if (!methodSets.count(methodName)) {

        // start a listener to wait for the reply
        const Scope replyScope = scope.concat(Scope("/" + methodName));
        ListenerPtr listener = getFactory().createListener(replyScope,
                listenerConfig);

        boost::shared_ptr<WaitingEventHandler> handler(
                new WaitingEventHandler(logger));
        listener->addHandler(handler);

        // informer for requests
        Informer<void>::Ptr informer = getFactory().createInformer<void>(
                scope.concat(Scope("/" + methodName)), informerConfig,
                sendType);

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

RemoteServer::FuturePtr RemoteServer::callAsync(const string& methodName,
        EventPtr data, ::rsb::HandlerPtr intermediateHandler) {

    RSCDEBUG(logger, "Calling method " << methodName << " with data " << data);

    // TODO check that the desired method exists
    MethodSet methodSet = getMethodSet(methodName, data->getType());
    FuturePtr result;
    {
        WaitingEventHandler::MutexType::scoped_lock
            lock(methodSet.handler->getMutex());

        data->setScopePtr(methodSet.requestInformer->getScope());
        data->setMethod("REQUEST");
        methodSet.requestInformer->publish(data);
        result.reset(new Future<EventPtr>());
        methodSet.handler->addCall(data->getEventId(), result,
                intermediateHandler);
    }

    return result;
}

EventPtr RemoteServer::call(const string& methodName,
                            EventPtr      data,
                            unsigned int  maxReplyWaitTime,
                            ::rsb::HandlerPtr intermediateHandler) {
    return callAsync(methodName, data, intermediateHandler)->get(
            maxReplyWaitTime);
}

}
}
