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

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>

#include "../ConverterSelectingConnector.h"

#include "rsb/rsbexports.h"

#include "Types.h"

namespace rsb {
namespace transport {
namespace socket {

// Forward declaration
class Bus;
typedef boost::shared_ptr<Bus> BusPtr;

/**
 * This class is intended to be used as a base class for connector
 * classes of the socket-based transport.
 *
 * It provides establishing and managing a connection to the bus via
 * an instance of the @ref Bus class.
 *
 * @author jmoringe
 */
class RSB_EXPORT ConnectorBase: public ConverterSelectingConnector<std::string> {
public:
    /**
     * Creates a connector for the given combination of @a host, @a
     * port and @a server.
     *
     * @param converters A strategy for converter selection within the
     *                   newly created connector.
     * @param host The host of the socket through which the newly
     *             created connector will operate.
     * @param port The port of the socket through which the newly
     *             created connector will operate.
     * @param server Controls whether the newly created connector
     *               should create a listening socket and wait for
     *               connections (value SERVER_YES), connect to an
     *               existing listen socket (value SERVER_NO) or try
     *               to automatically determine whether there already
     *               is a listening socket and create one only if
     *               necessary (value SERVER_AUTO).
     * @param tcpnodelay Controls whether the TCP_NODELAY socket
     *                   option should be set for the socket
     *                   implementing the communication of the newly
     *                   created connector. Setting this option trades
     *                   decreased latency for decreased throughput.
     */
    ConnectorBase(ConverterSelectionStrategyPtr converters,
                  const std::string&            host,
                  unsigned int                  port,
                  Server                        server,
                  bool                          tcpnodelay);

    virtual ~ConnectorBase();
protected:
    void activate();

    void deactivate();

    /**
     * Return the @ref Bus object through which this connector is
     * connected to a socket-based bus.
     *
     * @return a shared_pointer to the bus object.
     */
    BusPtr getBus();
private:
    rsc::logging::LoggerPtr logger;

    BusPtr                  bus;

    std::string             host;
    unsigned int            port;
    Server                  server;
    bool                    tcpnodelay;
};

typedef boost::shared_ptr<ConnectorBase> ConnectorBasePtr;

}
}
}
