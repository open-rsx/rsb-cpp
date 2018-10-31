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

#include <boost/format.hpp>

#include <rsc/misc/UUID.h>

#include "../EventId.h"
#include "../MetaData.h"
#include "../Factory.h"

#include "../filter/MethodFilter.h"

using namespace std;

using namespace boost;

using namespace rsc::runtime;
using namespace rsc::logging;
using namespace rsc::threading;

namespace rsb {
namespace patterns {

// RemoteMethod

RemoteServer::RemoteMethod::RemoteMethod(const Scope&             scope,
                                         const std::string&       name,
                                         const ParticipantConfig& listenerConfig,
                                         const ParticipantConfig& informerConfig)
    : Method(scope, name, listenerConfig, informerConfig),
      logger(Logger::getLogger(boost::str(boost::format("rsb.patterns.RemoteMethod[%1%]")
                                          % name))) {
}

RemoteServer::RemoteMethod::~RemoteMethod() {
}

std::string RemoteServer::RemoteMethod::getKind() const {
    return "remote-method";
}

ListenerPtr RemoteServer::RemoteMethod::makeListener() {
    ListenerPtr listener = Method::makeListener();
    listener->addFilter(filter::FilterPtr(new filter::MethodFilter("REPLY")));
    listener->addHandler(shared_from_this());
    return listener;
}

RemoteServer::FuturePtr RemoteServer::RemoteMethod::call(const std::string& /*methodName*/,
                                                         EventPtr           request) {
    FuturePtr result(new Future<EventPtr>());

    {
        MutexType::scoped_lock lock(this->inprogressMutex);

        request->setScopePtr(getInformer()->getScope());
        request->setMethod("REQUEST");
        getInformer()->publish(request);

        this->inprogress.insert(std::make_pair(request->getId(), result));
    }

    return result;
}

void RemoteServer::RemoteMethod::handle(EventPtr event) {
    if (!event || event->getCauses().empty()) {
        RSCTRACE(logger, "Received uninteresting event " << event);
        return;
    }

    EventId requestId = *event->getCauses().begin();
    RemoteServer::FuturePtr result;
    {
        MutexType::scoped_lock lock(this->inprogressMutex);
        map<EventId, FuturePtr>::const_iterator it
            = this->inprogress.find(requestId);
        if (it != this->inprogress.end()) {
            result = it->second;
            this->inprogress.erase(requestId);
        }
    }

    if (!result) {
        RSCTRACE(this->logger, "Received uninteresting event " << event);
        return;
    }
    RSCDEBUG(this->logger, "Received reply event " << event);

    if (event->mutableMetaData().hasUserInfo("rsb:error?")) {
        assert(event->getType() == typeName<std::string>());
        result->setError(boost::str(boost::format("Error calling remote method '%1%': %2%")
                                    % "TODO: obtain method name"
                                    % *(boost::static_pointer_cast<string>(event->getData()))));
    } else {
        result->set(event);
    }
}

// RemoteServer

RemoteServer::RemoteServer(const Scope&            scope,
                           const ParticipantConfig &listenerConfig,
                           const ParticipantConfig &informerConfig)
    : Participant(scope, listenerConfig), // TODO do this properly
      logger(Logger::getLogger(str(format("rsb.patterns.RemoteServer[%1%]")
                                   % scope.toString()))),
      listenerConfig(listenerConfig), informerConfig(informerConfig) {
    // TODO check that this server is alive...
    // TODO probably it would be a good idea to request some method infos from
    //      the server, e.g. for type checking
}

RemoteServer::~RemoteServer() {
    for (std::map<std::string, RemoteMethodPtr>::iterator it
             = this->methods.begin(); it != this->methods.end(); ++it) {
        it->second->deactivate();
    }
}

std::string RemoteServer::getKind() const {
    return "remote-server";
}

const std::set<std::string> RemoteServer::getTransportURLs() const {
    return std::set<std::string>();
}

RemoteServer::RemoteMethodPtr RemoteServer::getMethod(const string& name) {

    boost::mutex::scoped_lock lock(this->methodsMutex);

    if (this->methods.find(name) == this->methods.end()) {
        RemoteMethodPtr method
            = getFactory().createRemoteMethod(getScope()->concat(Scope("/" + name)),
                                              this->listenerConfig,
                                              this->informerConfig,
                                              this);

        method->activate();

        this->methods[name] = method;
    }

    return this->methods[name];

}

RemoteServer::FuturePtr RemoteServer::callAsync(const std::string& methodName,
                                                EventPtr           request) {
    RSCDEBUG(this->logger, "Calling method " << methodName << " with request " << request);

    // TODO check that the requested method exists
    return getMethod(methodName)->call(methodName, request);
}

EventPtr RemoteServer::call(const string& methodName,
                            EventPtr      request,
                            unsigned int  maxReplyWaitTime) {
    return callAsync(methodName, request)->get(maxReplyWaitTime);
}

}
}
