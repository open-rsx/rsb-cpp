/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2012, 2014 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
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

// Return the exception's what() string falling back to a replacement
// string in case what() throws an exception.
std::string safeSocketExceptionString(const std::exception& exception) {
    try {
        return exception.what();
    } catch (...) {
        return "<failed to determine exception message>";
    }
}

BusConnection::BusConnection(BusPtr    bus,
                             SocketPtr socket,
                             bool      client,
                             bool      tcpNoDelay) :
    logger(Logger::getLogger("rsb.transport.socket.BusConnection")),
    socket(socket), bus(bus), disconnecting(false), activeShutdown(false) {

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
    // only a safety measure. Usually we will not be destructed as long as
    // receiving is running.
    performSafeCleanup("destruction");
}

void BusConnection::shutdown() {
    RSCINFO(logger, "Shutting down");

    boost::recursive_mutex::scoped_lock lock(this->mutex);
    this->activeShutdown = true;

    if (this->socket && this->socket->is_open()) {
        this->socket->shutdown(ip::tcp::socket::shutdown_send);
    }

}

void BusConnection::disconnect() {
    RSCINFO(logger, "Disconnecting");

    boost::recursive_mutex::scoped_lock lock(this->mutex);
    this->disconnecting = true;

    if (this->socket && this->socket->is_open()) {
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

    {
        boost::recursive_mutex::scoped_lock lock(this->mutex);
        if (this->activeShutdown) {
            RSCDEBUG(this->logger, "Ignoring to send a notification "
                                   "because we are shutting down.");
            return;
        }

        // Send the size header, followed by the actual notification data.
        write(*this->socket, buffer(this->lengthSendBuffer));
        write(*this->socket, buffer(this->messageSendBuffer));
    }

}

void BusConnection::performSafeCleanup(const string& context) {
    // Remove ourselves from the bus to which we are connected.
    BusPtr bus = this->bus.lock();
    if (bus) {
        // The bus may already have removed its pointer.
        try {
            bus->removeConnection(shared_from_this());
        } catch (const boost::bad_weak_ptr&) {
        }
    }

    // We can only ignore the error here.
    if (!this->disconnecting) {
        try {
            disconnect();
        } catch (const std::exception& e) {
            // Use safeSocketExceptionString to avoid exceptions
            // potentially thrown when printing Boost.Asio exceptions.
            RSCDEBUG(this->logger, "Failed to disconnect (in " << context << "): "
                     << safeSocketExceptionString(e));
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

    {
        boost::recursive_mutex::scoped_lock lock(this->mutex);
        if (error == boost::asio::error::eof) {
            RSCDEBUG(logger, "Received eof");
            if (!this->activeShutdown) {
                try {
                    shutdown();
                } catch (const std::exception& e) {
                    // Use safeSocketExceptionString to avoid
                    // exceptions potentially thrown when printing
                    // Boost.Asio exceptions.
                    RSCWARN(this->logger, "Failed to shut down socket: "
                            << safeSocketExceptionString(e));
                }
            }
            performSafeCleanup("handleReadLength[eof]");
            return;
        }
    }

    if (error || (bytesTransferred != 4)) {
        if (!disconnecting) {
            RSCDEBUG(logger, "Receive failure (error " << error << ")"
                     << " or incomplete message header (received " << bytesTransferred << " bytes)"
                     << "; closing connection");
        }
        performSafeCleanup("handleReadLength[unknown error]");
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
        bus->handleIncoming(event, shared_from_this());
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
