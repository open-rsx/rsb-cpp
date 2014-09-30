/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2012 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "Bus.h"

#include <rsc/runtime/ContainerIO.h>

#include <boost/bind.hpp>

#include <boost/thread.hpp>

#include <boost/asio/ip/tcp.hpp>

#include "Factory.h"
#include "../../MetaData.h"

using namespace std;

using namespace rsc::logging;

using namespace boost::asio;
using boost::asio::ip::tcp;

namespace rsb {
namespace transport {
namespace socket {

Bus::Bus(io_service& service, bool tcpnodelay) :
    logger(Logger::getLogger("rsb.transport.socket.Bus")),
    service(service), tcpnodelay(tcpnodelay) {
}

Bus::~Bus() {
    RSCDEBUG(logger, "Destructing bus instance");

    // Sinks should be empty.
    if (!this->sinks.empty()) {
        RSCWARN(logger, "" << this->sinks.size() << " non-empty scopes when destructing");
    }

    // Active connections hold a shared_ptr to themselves and would
    // thus not be destructed. Disconnecting resolves this problem.
    for (ConnectionList::iterator it = this->connections.begin();
         it != this->connections.end(); ++it) {
        try {
            (*it)->shutdown();
        } catch (const std::exception& e) {
            RSCDEBUG(logger, "Failed to disconnect connection " << *it
                     << ": " << e.what());
        }
    }

    RSCDEBUG(logger, "Bus destruction finished");
}

bool Bus::isTcpnodelay() const {
    return this->tcpnodelay;
}

Bus::ConnectionList Bus::getConnections() const {
    return this->connections;
}

boost::recursive_mutex& Bus::getConnectionLock() {
    return this->connectionLock;
}

void Bus::addSink(InConnectorPtr sink) {
    boost::recursive_mutex::scoped_lock lock(this->connectorLock);

    Scope scope = sink->getScope();
    RSCDEBUG(logger, "Adding sink " << sink << " to scope " << scope);
    SinkList& connectors = this->sinks[scope];
    connectors.push_back(sink);
}

void Bus::removeSink(InConnector* sink) {
    boost::recursive_mutex::scoped_lock lock(this->connectorLock);

    Scope scope = sink->getScope();
    RSCDEBUG(logger, "Removing sink " << sink << " from scope " << scope);
    SinkList& connectors = this->sinks[scope];
    RSCDEBUG(logger, "Scope " << scope << " has "
             << connectors.size() << " connectors (before removing)");
    for (SinkList::iterator it = connectors.begin(); it != connectors.end(); ++it) {
        // If the weak pointer is dangling, we found our
        // sink. Otherwise, we can just check the pointer.
        InConnectorPtr ptr = it->lock();
        if (!ptr || (ptr.get() == sink)) {
            RSCDEBUG(logger, "Found connector " << sink << " in scope " << scope);
            connectors.erase(it);
            break;
        }
    }
    RSCDEBUG(logger, "Scope " << scope << " has "
             << connectors.size() << " connectors (after removing)");

    // If no connectors remain for the scope, the whole entry can be
    // removed.
    if (connectors.empty()) {
        RSCDEBUG(logger, "Removing empty scope " << scope);
        this->sinks.erase(scope);
    }
}

void Bus::addConnector(ConnectorBase* connector) {
    boost::recursive_mutex::scoped_lock lock(this->connectorLock);

    RSCDEBUG(logger, "Adding connector " << connector);
    this->connectors.push_back(connector);
}

void Bus::removeConnector(ConnectorBase* connector) {
    boost::recursive_mutex::scoped_lock lock(this->connectorLock);

    RSCDEBUG(logger, "Removing connector " << connector);
    this->connectors.remove(connector);

    // If no connectors remain, destroy the bus.
    if (this->connectors.empty()) {
        RSCINFO(logger, "No more connectors; suiciding");
        suicide();
    }
}

void Bus::addConnection(BusConnectionPtr connection) {
    RSCDEBUG(logger, "Adding connection " << connection);

    boost::recursive_mutex::scoped_lock lock(this->connectionLock);

    this->connections.push_back(connection);
}

void Bus::removeConnection(BusConnectionPtr connection) {
    RSCDEBUG(logger, "Removing connection " << connection);

    boost::recursive_mutex::scoped_lock lock(this->connectionLock);

    this->connections.remove(connection);
}

void Bus::handleIncoming(EventPtr         event,
                         BusConnectionPtr /*connection*/) {
    RSCDEBUG(logger, "Delivering received event to connectors " << event);

    vector<Scope> scopes = event->getScopePtr()->superScopes(true);
    RSCDEBUG(logger, "Relevant scopes " << scopes);

    {
        boost::recursive_mutex::scoped_lock lock(this->connectorLock);

        for (vector<Scope>::const_iterator it = scopes.begin(); it != scopes.end(); ++it) {
            SinkMap::const_iterator it_ = this->sinks.find(*it);
            if (it_ != this->sinks.end()) {
                const SinkList& connectors = it_->second;

                for (SinkList::const_iterator it__ = connectors.begin(); it__
                         != connectors.end(); ++it__) {
                    InConnectorPtr connector = it__->lock();
                    if (connector) {
                        RSCDEBUG(logger, "Delivering to connector " << connector << " in " << *it);
                        connector->handle(event);
                    }
                }
            }
        }
    }
}

void Bus::handle(EventPtr event) {
    // Dispatch to our own connectors.
    RSCDEBUG(logger, "Delivering outgoing event to connectors " << event);

    vector<Scope> scopes = event->getScopePtr()->superScopes(true);
    RSCDEBUG(logger, "Relevant scopes " << scopes);

    {
        boost::recursive_mutex::scoped_lock lock(this->connectorLock);

        for (vector<Scope>::const_iterator it = scopes.begin(); it != scopes.end(); ++it) {
            SinkMap::const_iterator it_ = this->sinks.find(*it);
            if (it_ != this->sinks.end()) {
                const SinkList& connectors = it_->second;

                for (SinkList::const_iterator it__ = connectors.begin(); it__
                         != connectors.end(); ++it__) {
                    InConnectorPtr connector = it__->lock();
                    if (connector) {
                        RSCDEBUG(logger, "Delivering to connector " << connector << " in " << *it);
                        connector->handle(event);
                    }
                }
            }
        }
    }

    // Dispatch to outgoing connections.
    {
        boost::recursive_mutex::scoped_lock lock(this->connectionLock);

        RSCDEBUG(logger, "Dispatching outgoing event " << event << " to connections");

        string wireSchema = event->getMetaData().getUserInfo("rsb.wire-schema");
        list<BusConnectionPtr> failing;
        for (list<BusConnectionPtr>::iterator it = this->connections.begin();
             it != this->connections.end(); ++it) {
            RSCDEBUG(logger, "Dispatching to connection " << *it);
            try {
                (*it)->sendEvent(event, wireSchema);
            } catch (const std::exception& e) {
                RSCWARN(logger, "Send failure (" << e.what() << "); will close connection later");
                // We record failing connections instead of closing them
                // immediately to avoid invalidating the iterator.
                failing.push_back(*it);
            }
        }

        // This should remove all references to the connection objects.
        for (list<BusConnectionPtr>::const_iterator it = failing.begin();
             it != failing.end(); ++it) {
            removeConnection(*it);
        }
    }
}

void Bus::suicide() {
    Factory::getInstance().removeBusClient(shared_from_this());
}

void Bus::printContents(ostream& stream) const {
    stream << "connections = " << this->connections
           << ", sinks = " << this->sinks;
}

}
}
}
