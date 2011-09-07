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

#include "BusServer.h"

#include <boost/bind.hpp>

using namespace boost;

using namespace boost::asio;
using boost::asio::ip::tcp;

using namespace rsc::logging;

namespace rsb {
namespace transport {
namespace socket {

Listener::Listener(Bus        &bus,
                   uint16_t    port,
                   io_service &service)
    : logger(Logger::getLogger("rsb.transport.socket.Listener")),
      bus(bus),
      acceptor(service, tcp::endpoint(tcp::v4(), port)),
      service(service) {
    acceptOne();
}

void Listener::acceptOne() {
    SocketPtr socket(new tcp::socket(this->service));

    acceptor.async_accept(*socket,
                          boost::bind(&Listener::handleAccept, this, socket,
                                      boost::asio::placeholders::error));
}

void Listener::handleAccept(SocketPtr                 socket,
                            const system::error_code &error) {
    if (!error) {
        //
        RSCINFO(logger, "Got connection from " << socket->remote_endpoint());

        BusConnectionPtr connection(new BusConnection(this->bus.shared_from_this(), socket));
        connection->receiveEvent();
        this->bus.addConnection(connection);
    } else {
        RSCWARN(logger, "Accept failure, trying to continue")
    }

    // Continue accepting connections.
    acceptOne();
}

BusServer::BusServer(uint16_t    port,
                     io_service &service)
    : Bus(service),
      listener(*this, port, service) {
}

}
}
}
