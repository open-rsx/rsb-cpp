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

#include "Server.h"

#include <stdexcept>

#include <rsc/runtime/TypeStringTools.h>
#include <rsc/logging/Logger.h>

#include "../EventId.h"
#include "../Factory.h"
#include "../MetaData.h"
#include "../Handler.h"
#include "MethodExistsException.h"

using namespace std;

using namespace rsc::runtime;

namespace rsb {
namespace patterns {

Server::IntlCallback::~IntlCallback() {
}

Server::CallbackBase::CallbackBase(const string& requestType,
                                   const string& replyType)
    : requestType(requestType), replyType(replyType) {
}

const string& Server::CallbackBase::getRequestType() const {
    return this->requestType;
}

const string& Server::CallbackBase::getReplyType() const {
    return this->replyType;
}

class Server::DefaultIntermediateResultHandler: public IntermediateResultHandler {
public:

    DefaultIntermediateResultHandler(Informer<AnyType>::Ptr informer,
            const EventId& cause) :
            informer(informer), cause(cause) {
    }

    virtual ~DefaultIntermediateResultHandler() {
    }

    virtual void publish(const AnnotatedData& data) {
        EventPtr event(new Event());
        event->setScopePtr(informer->getScope());
        event->addCause(cause);
        event->setType(data.first);
        event->setData(data.second);
        // the client has to decide what to do with exceptions. We cannot know
        informer->publish(event);
    }

private:

    Informer<AnyType>::Ptr informer;
    EventId cause;

};

class Server::RequestHandler: public Handler {
private:

    rsc::logging::LoggerPtr logger;

    string methodName;
    Server::CallbackPtr callback;
    Informer<AnyType>::Ptr informer;

public:

    RequestHandler(const string& methodName, Server::CallbackPtr callback,
            Informer<AnyType>::Ptr informer) :
        logger(rsc::logging::Logger::getLogger("rsb.patterns.RequestHandler."
                + methodName)), methodName(methodName), callback(callback),
                informer(informer) {
    }

    string getClassName() const {
        return "RequestHandler";
    }

    void printContents(ostream& stream) const {
        stream << "methodName = " << methodName;
    }

    void handle(EventPtr event) {
        if (event->getMethod() != "REQUEST") {
            return;
        }
        if (event->getType() != callback->getRequestType()) {
            RSCERROR(logger, "Request type '" << event->getType()
                    << "' does not match expected request type '"
                    << callback->getRequestType() << "' of method '"
                    << methodName << "'");
            return;
        }

        // now let the callback execute and prepare the result event from this
        EventPtr reply(new Event());
        reply->setScopePtr(informer->getScope());
        reply->setMethod("REPLY");
        reply->addCause(event->getEventId());
        try {
            CallSpec spec;
            spec.methodName = methodName;
            spec.data = event->getData();
            spec.intermediateResultHandler.reset(
                    new DefaultIntermediateResultHandler(informer,
                            event->getEventId()));
            AnnotatedData returnData = callback->intlCall(spec);
            reply->setType(returnData.first);
            reply->setData(returnData.second);
        } catch (const exception& e) {
            reply->setType(typeName<string>());
            reply->setData(
                    boost::shared_ptr<string>(
                            new string(typeName(e) + ": " + e.what())));
            reply->mutableMetaData().setUserInfo("rsb:error?", "");
        }
        informer->publish(reply);
    }

};

Server::Server(const Scope& scope, const ParticipantConfig &listenerConfig,
        const ParticipantConfig &informerConfig) :
        scope(scope), listenerConfig(listenerConfig), informerConfig(
                informerConfig) {
}

Server::~Server() {
}

void Server::registerMethod(const std::string& methodName, CallbackPtr callback) {

    // check that method does not exist
    if (methods.count(methodName)) {
        throw MethodExistsException(methodName, scope.toString());
    }

    // TODO check that the reply type is convertible
    Informer<AnyType>::Ptr informer = getFactory().createInformer<AnyType>(
            scope.concat(Scope("/" + methodName)), informerConfig, "");

    ListenerPtr listener = getFactory().createListener(
            scope.concat(Scope("/" + methodName)), listenerConfig);
    listener->addHandler(
            HandlerPtr(new RequestHandler(methodName, callback, informer)));
    this->requestListeners.insert(listener);

    methods[methodName] = informer;

}

Server::Callback<void, void>::Callback() :
        Server::CallbackBase(rsc::runtime::typeName(typeid(void)),
                rsc::runtime::typeName(typeid(void))) {
}

void Server::Callback<void, void>::call(const std::string& /*methodName*/) {
    throw std::logic_error(
            "Either implement this method or the one with more parameters");
}

void Server::Callback<void, void>::call(const std::string& methodName,
        IntermediateResultHandlerPtr /*intermediateResultHandler*/) {
    call(methodName);
}

AnnotatedData Server::Callback<void, void>::intlCall(const CallSpec& spec) {
    call(spec.methodName);
    return std::make_pair(getReplyType(), boost::shared_ptr<void>());
}

}
}
