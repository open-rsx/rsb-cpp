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

#include "LocalServer.h"

#include <stdexcept>

#include "../EventId.h"
#include "../MetaData.h"
#include "../Factory.h"

#include "../filter/MethodFilter.h"

#include "MethodExistsException.h"

using namespace std;

using namespace rsc::runtime;

namespace rsb {
namespace patterns {

// Callbacks

LocalServer::IntlCallback::~IntlCallback() {
}

LocalServer::CallbackBase::CallbackBase(const string& requestType,
                                   const string& replyType)
    : requestType(requestType), replyType(replyType) {
}

const string& LocalServer::CallbackBase::getRequestType() const {
    return this->requestType;
}

const string& LocalServer::CallbackBase::getReplyType() const {
    return this->replyType;
}

EventPtr LocalServer::EventCallback::intlCall(const string& methodName, EventPtr request) {
    return call(methodName, request);
}

// LocalMethod

LocalServer::LocalMethod::LocalMethod(const Scope&             scope,
                                      const std::string&       name,
                                      const ParticipantConfig& listenerConfig,
                                      const ParticipantConfig& informerConfig,
                                      CallbackPtr              callback)
    : Method(scope, name, listenerConfig, informerConfig),
      logger(rsc::logging::Logger::getLogger(boost::str(boost::format("rsb.patterns.LocalMethod[%1%]")
                                                        % name))),
      callback(callback) {
}

LocalServer::LocalMethod::~LocalMethod() {
}

std::string LocalServer::LocalMethod::getKind() const {
    return "local-method";
}

ListenerPtr LocalServer::LocalMethod::makeListener() {
    ListenerPtr listener = Method::makeListener();
    listener->addFilter(filter::FilterPtr(new filter::MethodFilter("REQUEST")));
    listener->addHandler(HandlerPtr(shared_from_this()));
    return listener;
}

void LocalServer::LocalMethod::handle(EventPtr event) {
    LocalServer::CallbackBase* callbackWithReturnType
        = dynamic_cast<LocalServer::CallbackBase*>(this->callback.get());
    if (callbackWithReturnType) {
        if (event->getType() != callbackWithReturnType->getRequestType()) {
            RSCERROR(this->logger, boost::format("Request type '%1%' "
                                                 "does not match expected request type '%2%' "
                                                 "of method '%3%'")
                     % event->getType()
                     % callbackWithReturnType->getRequestType()
                     % getName());
            return;
        }
    }

    EventPtr reply;
    try {
        reply = this->callback->intlCall(getName(), event);
        assert(reply);
    } catch (const exception& e) {
        reply.reset(new Event());
        reply->setType(typeName<string>());
        reply->setData(boost::shared_ptr<string>(new string(typeName(e) + ": " + e.what())));
        reply->mutableMetaData().setUserInfo("rsb:error?", "");
    }
    reply->setScopePtr(getInformer()->getScope());
    reply->setMethod("REPLY");
    reply->addCause(event->getId());
    getInformer()->publish(reply);
}

// LocalServer

LocalServer::LocalServer(const Scope&             scope,
                         const ParticipantConfig &listenerConfig,
                         const ParticipantConfig &informerConfig)
    : Participant(scope, listenerConfig), // TODO do this properly
      listenerConfig(listenerConfig),
      informerConfig(informerConfig) {
}

LocalServer::~LocalServer() {
    for (std::map<std::string, LocalMethodPtr>::iterator it
             = this->methods.begin(); it != this->methods.end(); ++it) {
        it->second->deactivate();
    }
}

std::string LocalServer::getKind() const {
    return "local-server";
}

void LocalServer::registerMethod(const std::string& name, CallbackPtr callback) {

    // TODO locking?

    // check that method does not exist
    if (this->methods.count(name)) {
        throw MethodExistsException(name, getScope()->toString());
    }

    // TODO check that the reply type is convertible
    LocalMethodPtr method
        = getFactory().createLocalMethod(getScope()->concat(Scope("/" + name)),
                                         callback,
                                         this->listenerConfig, this->informerConfig,
                                         shared_from_this());
    method->activate();

    this->methods[name] = method;

}

}
}
