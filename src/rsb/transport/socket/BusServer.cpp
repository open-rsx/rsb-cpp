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

#include "Factory.h"

using namespace boost;

using namespace boost::asio;
using boost::asio::ip::tcp;

using namespace rsc::logging;

namespace rsb {
namespace transport {
namespace socket {

BusServer::BusServer(uint16_t    port,
                     io_service& service)
    : Bus(service),
      logger(Logger::getLogger("rsb.transport.socket.BusServer")),
      acceptor(service, tcp::endpoint(tcp::v4(), port)),
      service(service) {
    acceptOne();
}

void BusServer::acceptOne() {
    SocketPtr socket(new tcp::socket(this->service));

    RSCINFO(logger, "Listening on " << this->acceptor.local_endpoint());
    acceptor.async_accept(*socket,
                          bind(&BusServer::handleAccept, this, socket,
                               placeholders::error));
}

void BusServer::handleAccept(SocketPtr                 socket,
                             const boost::system::error_code& error) {
    if (!error) {
        //
        RSCINFO(logger, "Got connection from " << socket->remote_endpoint());

        BusConnectionPtr connection(new BusConnection(shared_from_this(), socket, false));
        addConnection(connection);
        connection->startReceiving();
    } else {
        RSCWARN(logger, "Accept failure, trying to continue");
    }

    // Continue accepting connections.
    acceptOne();
}

void BusServer::handleIncoming(EventPtr event) {
    Bus::handleIncoming(event);

    RSCDEBUG(logger, "Delivering received event to connections " << event);
    /** TODO(jmoringe): implement */
    /*for (ConnectionList::iterator it = this->connections.begin();
         it != this->connections.end(); ++it) {
        it->sendEvent(event, event->getMetaData().getUserInfo("rsb.wire-schema"));
        }*/

}

void BusServer::suicide() {
    Factory::getInstance().removeBusServer(shared_from_this());
}

}
}
}
