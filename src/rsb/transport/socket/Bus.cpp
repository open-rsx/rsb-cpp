/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "Bus.h"

#include <rsc/runtime/ContainerIO.h>

#include <boost/bind.hpp>

#include <boost/thread.hpp>

#include <boost/asio/ip/tcp.hpp>

#include "Factory.h"
#include "../../MetaData.h"

using namespace std;

using namespace boost;

using namespace rsc::logging;

using namespace boost::asio;
using boost::asio::ip::tcp;

namespace rsb {
namespace transport {
namespace socket {

Bus::Bus(io_service& service) :
    logger(Logger::getLogger("rsb.transport.socket.Bus")),
    service(service) {
}

Bus::~Bus() {
    RSCDEBUG(logger, "Destroying");

    // Sinks should be empty.
    if (!this->sinks.empty()) {
        RSCWARN(logger, "" << this->sinks.size() << " non-empty scopes when destructing");
    }

    // Active connections hold a shared_ptr to themselves and would
    // thus not be destructed.
    for (ConnectionList::iterator it = this->connections.begin();
         it != this->connections.end(); ++it) {
        try {
            (*it)->disconnect();
        } catch (const std::exception& e) {
            RSCERROR(logger, "Failed to disconnect connection " << *it
                     << ": " << e.what());
        }
    }
}

void Bus::addSink(InPushConnectorPtr sink) {
    recursive_mutex::scoped_lock lock(this->connectorLock);

    Scope scope = sink->getScope();
    RSCDEBUG(logger, "Adding sink " << sink << " to scope " << scope);
    SinkList& connectors = this->sinks[scope];
    connectors.push_back(sink);
}

void Bus::removeSink(InPushConnector* sink) {
    recursive_mutex::scoped_lock lock(this->connectorLock);

    Scope scope = sink->getScope();
    RSCDEBUG(logger, "Removing sink " << sink << " from scope " << scope);
    SinkList& connectors = this->sinks[scope];
    RSCDEBUG(logger, "Scope " << scope << " has "
             << connectors.size() << " connectors (before removing)");
    for (SinkList::iterator it = connectors.begin(); it != connectors.end(); ++it) {
        // If the weak pointer is dangling, we found our
        // sink. Otherwise, we can just check the pointer.
        InPushConnectorPtr ptr = it->lock();
        if (!ptr || (ptr.get() == sink)) {
            RSCDEBUG(logger, "Found connector " << sink << " in scope " << scope);
            connectors.erase(it);
            break;
        }
    }
    RSCDEBUG(logger, "Scope " << scope << " has "
             << connectors.size() << " connectors (after removing)");
    if (connectors.empty()) {
        RSCDEBUG(logger, "Removing empty scope " << scope);
        this->sinks.erase(scope);
    }
}

void Bus::addConnector(/*ConnectorBasePtr*/ConnectorBase* connector) {
    recursive_mutex::scoped_lock lock(this->connectorLock);

    RSCDEBUG(logger, "Adding connector " << connector);
    this->connectors.push_back(connector);
}

void Bus::removeConnector(/*ConnectorBasePtr*/ConnectorBase* connector) {
    recursive_mutex::scoped_lock lock(this->connectorLock);

    RSCDEBUG(logger, "Removing connector " << connector);
    this->connectors.remove(connector);

    if (this->connectors.empty()) {
        RSCINFO(logger, "No more connectors; suiciding");
        suicide();
    }
}

void Bus::addConnection(BusConnectionPtr connection) {
    RSCDEBUG(logger, "Adding connection " << connection);

    recursive_mutex::scoped_lock lock(this->connectionLock);

    this->connections.push_back(connection);
}

void Bus::removeConnection(BusConnectionPtr connection) {
    RSCDEBUG(logger, "Removing connection " << connection);

    recursive_mutex::scoped_lock lock(this->connectionLock);

    this->connections.remove(connection);
}

void Bus::handleIncoming(EventPtr event) {
    RSCDEBUG(logger, "Delivering received event to connectors " << event);

    vector<Scope> scopes = event->getScopePtr()->superScopes(true);
    RSCDEBUG(logger, "Relevant scopes " << scopes);

    {
        recursive_mutex::scoped_lock lock(this->connectorLock);

        for (vector<Scope>::const_iterator it = scopes.begin(); it != scopes.end(); ++it) {
            SinkMap::const_iterator it_ = this->sinks.find(*it);
            if (it_ != this->sinks.end()) {
                const SinkList& connectors = it_->second;

                for (SinkList::const_iterator it__ = connectors.begin(); it__
                         != connectors.end(); ++it__) {
                    InPushConnectorPtr connector = it__->lock();
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
    recursive_mutex::scoped_lock lock(this->connectionLock);

    RSCDEBUG(logger, "Dispatching outgoing event " << event << " to connections");

    string wireSchema = event->getMetaData().getUserInfo("rsb.wire-schema");
    for (list<BusConnectionPtr>::iterator it = this->connections.begin();
	 it != this->connections.end(); ++it) {
        RSCDEBUG(logger, "Dispatching to connection " << *it);
        (*it)->sendEvent(event, wireSchema);
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
