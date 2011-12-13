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

#pragma once

#include <map>

#include <boost/thread/thread.hpp>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <rsc/logging/Logger.h>

#include <rsc/patterns/Singleton.h>

#include "Bus.h"
#include "BusServer.h"

namespace rsb {
namespace transport {
namespace socket {

/**
 * The singleton instance of this class is responsible for managing
 * bus provider objects.
 *
 * For each endpoint (i.e. address and port), at most one client or
 * server bus provider can exist.
 *
 * @author jmoringe
 */
class RSB_EXPORT Factory : public rsc::patterns::Singleton<Factory> {
    friend class rsc::patterns::Singleton<Factory>;
    friend class Bus;       // for private member function removeBusClient
    friend class BusServer; // for private member function removeBusServer
public:
    ~Factory();

    BusPtr getBusClientFor(const std::string& host,
                           boost::uint16_t    port,
                           bool               tcpnodelay,
                           ConnectorBase*     connector);

    BusServerPtr getBusServerFor(const std::string& host,
                                 boost::uint16_t    port,
                                 bool               tcpnodelay,
                                 ConnectorBase*     connector);
private:
    typedef std::pair<std::string, boost::uint16_t>	     Endpoint;
    typedef boost::shared_ptr<boost::asio::ip::tcp::socket>  SocketPtr;

    typedef boost::shared_ptr<boost::asio::io_service::work> WorkPtr;

    typedef std::map<Endpoint, BusPtr>			     BusClientMap;
    typedef std::map<Endpoint, BusServerPtr>		     BusServerMap;

    rsc::logging::LoggerPtr logger;

    BusClientMap            busClients;
    BusServerMap            busServers;
    /** TODO(jmoringe): locking */

    boost::asio::io_service service;
    WorkPtr                 keepAlive;
    boost::thread           thread;

    Factory();

    void removeBusClient(BusPtr bus);

    void removeBusServer(BusPtr bus);

    static void checkOptions(BusPtr bus, bool tcpnodelay);
};

}
}
}
