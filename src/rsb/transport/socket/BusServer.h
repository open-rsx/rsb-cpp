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

#include <rsc/logging/Logger.h>

#include "Bus.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {
namespace socket {

class RSB_EXPORT Listener {
public:
    typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;

    Listener(Bus&                     bus,
             boost::uint16_t          port,
             boost::asio::io_service &service);

private:
    void acceptOne();

    void handleAccept(SocketPtr                       socket,
                      const boost::system::error_code &error);

    rsc::logging::LoggerPtr         logger;

    Bus&                            bus;

    boost::asio::ip::tcp::acceptor  acceptor;
    boost::asio::io_service        &service;
};

/** @TODO(jmoringe): document
 *
 *
 * @author jmoringe
 */
class RSB_EXPORT BusServer : public Bus {
public:
    BusServer(boost::uint16_t          port,
              boost::asio::io_service &service);

private:
    Listener listener;
};

typedef boost::shared_ptr<BusServer> BusServerPtr;

}
}
}
