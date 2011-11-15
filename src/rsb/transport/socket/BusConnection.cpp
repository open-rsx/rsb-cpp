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

#include "BusConnection.h"

#include <boost/bind.hpp>

#include <rsc/misc/langutils.h>

#include "Bus.h"
#include "Serialization.h"

using namespace std;

using namespace boost;

using namespace boost::asio;
//using tcp = boost::asio::ip::tcp;

using namespace rsc::logging;

namespace rsb {
namespace transport {
namespace socket {

BusConnection::BusConnection(BusPtr    bus,
                             SocketPtr socket,
                             bool      client,
                             bool      tcpNoDelay) :
    logger(Logger::getLogger("rsb.transport.socket.BusConnection")),
    socket(socket), bus(bus), disconnecting(false) {

    // Enable TCPNODELAY socket option to trade decreased throughput
    // for reduced latency.
    if (tcpNoDelay){
        RSCINFO(logger, "Setting TCP_NODELAY option");
        boost::asio::ip::tcp::no_delay option(true);
        socket->set_option(option);
    }

    // Allocate static buffers.
    this->lengthReceiveBuffer.resize(4);
    this->lengthSendBuffer.resize(4);

    // Perform request role of the handshake.
    if (client) {
        read(*this->socket, buffer(&this->lengthReceiveBuffer[0], 4));
    } else {
        write(*this->socket, buffer(this->lengthSendBuffer));
    }
}

BusConnection::~BusConnection() {
    performSafeCleanup("destructor");
}

void BusConnection::disconnect() {
    RSCINFO(logger, "Disconnecting");
    this->disconnecting = true;

    if (this->socket && this->socket->is_open()) {
        this->socket->shutdown(ip::tcp::socket::shutdown_send);
        this->socket->shutdown(ip::tcp::socket::shutdown_receive);
        RSCINFO(logger, "Closing");
        this->socket->close();
    }
}

void BusConnection::startReceiving() {
    receiveEvent();
}

void BusConnection::sendEvent(EventPtr      event,
                              const string& wireSchema) {
    // Convert the event into a notification object and serialize the
    // notification object.
    // The payload already is a byte-array, since it has been
    // serialized by the connector which submitted the event.
    protocol::Notification notification;
    eventToNotification(notification, event, wireSchema,
                        *static_pointer_cast<string>(event->getData()));
    notification.SerializeToString(&this->messageSendBuffer);

    // Encode the size of the serialized notification object.
    uint32_t length = this->messageSendBuffer.size();
    this->lengthSendBuffer[0] = (length & 0x000000fful) >> 0;
    this->lengthSendBuffer[1] = (length & 0x0000ff00ul) >> 8;
    this->lengthSendBuffer[2] = (length & 0x00ff0000ul) >> 16;
    this->lengthSendBuffer[3] = (length & 0xff000000ul) >> 24;

    // Send the size header, followed by the actual notification data.
    write(*this->socket, buffer(this->lengthSendBuffer));
    write(*this->socket, buffer(this->messageSendBuffer));
}

void BusConnection::performSafeCleanup(const string& context) {
    // Remove ourselves from the bus to which we are connected.
    BusPtr bus = this->bus.lock();
    if (bus) {
        // The bus may already have removed its pointer.
        try {
            bus->removeConnection(shared_from_this());
        } catch (const boost::bad_weak_ptr& e) {
        }
    }

    // We can only ignore the error here.
    if (!this->disconnecting) {
        try {
            disconnect();
        } catch (const std::exception& e) {
            RSCERROR(logger, "Failed to disconnect (in " << context << "): "
                     << e.what());
        }
    }
}

void BusConnection::receiveEvent() {
    async_read(*this->socket,
               buffer(&this->lengthReceiveBuffer[0], 4),
               boost::bind(&BusConnection::handleReadLength, shared_from_this(),
                           boost::asio::placeholders::error,
                           boost::asio::placeholders::bytes_transferred));
}

void BusConnection::handleReadLength(const boost::system::error_code& error,
                                     size_t                    bytesTransferred) {
    if (error || (bytesTransferred != 4)) {
        if (!disconnecting) {
            RSCWARN(logger, "Receive failure (error " << error << ")"
                    << " or incomplete message header (received " << bytesTransferred << " bytes)"
                    << "; closing connection");
        }
        performSafeCleanup("handleReadLength");
        return;
    }

    uint32_t size
        = (((uint32_t) *reinterpret_cast<unsigned char*>(&this->lengthReceiveBuffer[0])) << 0)
        | (((uint32_t) *reinterpret_cast<unsigned char*>(&this->lengthReceiveBuffer[1])) << 8)
        | (((uint32_t) *reinterpret_cast<unsigned char*>(&this->lengthReceiveBuffer[2])) << 16)
        | (((uint32_t) *reinterpret_cast<unsigned char*>(&this->lengthReceiveBuffer[3])) << 24);

    RSCDEBUG(logger, "Received message header with size " << size);

    this->messageReceiveBuffer.resize(size);

    async_read(*this->socket,
               buffer(&this->messageReceiveBuffer[0], size),
               boost::bind(&BusConnection::handleReadBody, shared_from_this(),
                           boost::asio::placeholders::error,
                           boost::asio::placeholders::bytes_transferred,
                           size));
}

void BusConnection::handleReadBody(const boost::system::error_code& error,
                                   size_t                    bytesTransferred,
                                   size_t expected) {
    if (error || (bytesTransferred != expected)) {
        if (!this->disconnecting) {
            RSCWARN(logger, "Receive failure (error " << error << ")"
                    << " or incomplete message body (received " << bytesTransferred << " bytes)"
                    << "; closing connection");
        }
        performSafeCleanup("handleReadBody");
        return;
    }

    // Deserialize the notification.
    this->notification.ParseFromString(this->messageReceiveBuffer);

    // Construct an Event instance *without* deserializing the
    // payload. This has to be done in connectors since different
    // converters can be used.
    EventPtr event = notificationToEvent(this->notification, true);

    // Dispatch the received event to connectors.
    BusPtr bus = this->bus.lock();
    if (bus) {
        bus->handleIncoming(event);
    } else {
        RSCWARN(logger, "Dangling bus pointer when trying to dispatch incoming event; closing connection");
        performSafeCleanup("handleReadBody");
        return;
    }

    // Submit task to start receiving the next event.
    receiveEvent();
}

void BusConnection::printContents(ostream& stream) const {
    try {
        stream << "local = " << this->socket->local_endpoint()
               << ", remote = " << this->socket->remote_endpoint();
    } catch (...) {
        stream << "<error printing socket info>";
    }
}

}
}
}
