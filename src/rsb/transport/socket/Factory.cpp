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

#include "Factory.h"

#include <boost/lexical_cast.hpp>

#include <boost/asio/ip/address.hpp>

using namespace std;

using namespace boost;

using namespace boost::asio;
using boost::asio::ip::tcp;

using namespace rsc::logging;

namespace rsb {
namespace transport {
namespace socket {

// Create and start an io_service. This service will be shared between
// all bus providers created by this factory..
Factory::Factory() :
    logger(Logger::getLogger("rsb.transport.socket.Factory")),
    keepAlive(new io_service::work(service)),
    thread(boost::bind(&boost::asio::io_service::run, &service)) {
    RSCINFO(logger, "Started service thread");
}

Factory::~Factory() {
    if (!this->busClients.empty()) {
        RSCWARN(logger, "Remaining bus clients: " << this->busClients);
    }

    RSCINFO(logger, "Stopping service thread");
    this->keepAlive.reset();
    //this->service.stop();
    this->thread.join();
    RSCINFO(logger, "Stopped service thread");
}

BusPtr Factory::getBusClientFor(const string  &host,
                                uint16_t       port,
                                ConnectorBase *connector) {
    RSCDEBUG(logger, "Was asked for a bus client for " << host << ":" << port);

    // Try to find an entry for the exact specified endpoint. If this
    // yields a hit, there is no need to resolve the specified name.
    {
        Endpoint endpoint(host, port);

        BusClientMap::const_iterator it;
        if ((it = this->busClients.find(endpoint)) != this->busClients.end()) {
            BusPtr result = it->second; //.lock();
            if (result) {
                RSCDEBUG(logger, "Found existing bus client  "
                         << result << " without resolving";)

                return result;
            } else {
                RSCDEBUG(logger, "Dangling bus client pointer without resolving");
            }
        }

        RSCDEBUG(logger, "Did not find bus client without resolving");
    }

    // We did not find an entry for the exact specified entry. We try
    // to resolve it to a working endpoint and use that one in the
    // lookup.
    // TODO(jmoringe): avoid this useless socket connection just for
    // the lookup
    RSCDEBUG(logger, "Resolving endpoint")
    tcp::resolver resolver(this->service);
    tcp::resolver::query query(host, lexical_cast<string>(port),
                               tcp::resolver::query::numeric_service);
    tcp::resolver::iterator endpointIterator = resolver.resolve(query);

    SocketPtr socket(new tcp::socket(this->service));

    /** TODO(jmoringe): try remaining endpoints if this fails */
    RSCDEBUG(logger, "Trying endpoint " << endpointIterator->endpoint());
    socket->connect(*endpointIterator);
    //tcp::endpoint e(ip::address_v4::from_string(host), port);
    //socket->connect(e);
    RSCDEBUG(logger, "Connected");

    // When we have a working endpoint, repeat the lookup. Create a
    // new bus client, if there is still no entry.
    {
        Endpoint endpoint(endpointIterator->host_name(), port);

        BusClientMap::const_iterator it;
        if ((it = this->busClients.find(endpoint)) != this->busClients.end()) {
            BusPtr result = it->second; //.lock();
            if (result) {
                RSCDEBUG(logger, "Found existing bus client " << it->second << " after resolving");

                return result;
            } else {
                RSCDEBUG(logger, "Dangling bus client pointer after resolving");
            }
        }

        RSCDEBUG(logger, "Did not find bus client after resolving; creating a new one");

        BusPtr result(new Bus(this->service));
        BusConnectionPtr connection(new BusConnection(result, socket, true));
        result->addConnection(connection);
        connection->receiveEvent();
        this->busClients[endpoint] = result;

        RSCDEBUG(logger, "Created new bus client " << result);

        return result;
    }
}

void Factory::removeBusClient(BusPtr bus) {
    RSCDEBUG(logger, "Removing bus " << bus);

    for (BusClientMap::iterator it = this->busClients.begin()
             ; it != this->busClients.end(); ++it) {
        if (it->second == bus) {
            this->busClients.erase(it);
            RSCDEBUG(logger, "Removed");
            return;
        }
    }
}

BusServerPtr Factory::getBusServerFor(const string &host,
                                      uint16_t      port,
                                      ConnectorBase     *connector) {
    RSCDEBUG(logger, "Was asked for a bus server for " << host << ":" << port);

    // Try to find an existing entry for the specified endpoint.
    Endpoint endpoint(host, port);

    BusServerMap::const_iterator it;
    if ((it = this->busServers.find(endpoint)) != this->busServers.end()) {
        RSCDEBUG(logger, "Found existing bus server " << it->second);
        return it->second;
    }

    // If there is no entry, create a new bus server and put it into
    // the map.
    RSCDEBUG(logger, "Did not find bus server; creating a new one");

    BusServerPtr result(new BusServer(port, this->service));
    this->busServers[endpoint] = result;

    RSCDEBUG(logger, "Created new bus client " << result);

    return result;
}

void Factory::removeBusServer(BusPtr bus) {
}

}
}
}
