/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2012 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <boost/cstdint.hpp>

#include <boost/shared_ptr.hpp>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <rsc/logging/Logger.h>

#include "Bus.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {
namespace socket {

/**
 * Instances of this class provide access to a socket-based bus for
 * local and remote bus clients.
 *
 * Remote clients can connect to a server socket in order to send and
 * receive events through the resulting socket connection.
 *
 * Local clients (connectors) use the usual @ref Bus interface to
 * receive events submitted by remote clients and submit events which
 * will be distributed to remote clients by the @ref BusServer.
 *
 * @author jmoringe
 */
class RSB_EXPORT BusServer : public Bus {
public:
    BusServer(boost::uint16_t          port,
              bool                     tcpnodelay,
              boost::asio::io_service& service);

    virtual ~BusServer();

    void handleIncoming(EventPtr         event,
                        BusConnectionPtr connection);

private:
    typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;

    rsc::logging::LoggerPtr         logger;

    boost::asio::ip::tcp::acceptor  acceptor;
    boost::asio::io_service&        service;

    volatile bool                   shutdown;

    void acceptOne();

    void handleAccept(SocketPtr                       socket,
                      const boost::system::error_code& error);

    void suicide();
};

typedef boost::shared_ptr<BusServer> BusServerPtr;

}
}
}
