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

#include <string>

#include <boost/enable_shared_from_this.hpp>

#include <boost/asio.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/runtime/Printable.h>

#include "../../Event.h"

#include "../../protocol/Notification.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {
namespace socket {

// forward declaration
class Bus;
typedef boost::shared_ptr<Bus> BusPtr;

class RSB_EXPORT BusConnection : public boost::enable_shared_from_this<BusConnection>,
                                 public rsc::runtime::Printable {
public:
    typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;

    BusConnection(BusPtr    bus,
                  SocketPtr socket);

    void receiveEvent();

    void sendEvent(EventPtr           event,
                   const std::string &wireSchema);
private:
    rsc::logging::LoggerPtr logger;

    SocketPtr               socket;

    BusPtr                  bus;

    // Receive buffers
    protocol::Notification  notification;
    std::string             lengthReceiveBuffer;
    std::string             messageReceiveBuffer;

    // Send buffers
    std::string             lengthSendBuffer;
    std::string             messageSendBuffer;

    void handleReadLength(const boost::system::error_code &error,
                          size_t                           bytesTransferred);

    void handleReadBody(const boost::system::error_code &error,
                        size_t                           bytesTransferred);

    void handleWriteLength(const boost::system::error_code &error,
                           size_t                           bytesTransferred);

    void handleWriteBody(const boost::system::error_code &error,
                         size_t                           bytesTransferred);

    void fillNotification(protocol::Notification &notification,
                          const EventPtr         &event,
                          const std::string      &wireSchema,
                          const std::string      &data);

    void printContents(std::ostream &stream) const;
};

typedef boost::shared_ptr<BusConnection> BusConnectionPtr;

}
}
}
