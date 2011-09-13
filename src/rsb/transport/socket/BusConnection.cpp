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

using namespace std;

using namespace boost;

using namespace boost::asio;

using namespace rsc::logging;

namespace rsb {
namespace transport {
namespace socket {

BusConnection::BusConnection(BusPtr    bus,
                             SocketPtr socket) :
    logger(Logger::getLogger("rsb.transport.socket.BusConnection")),
    socket(socket), bus(bus) {
    // Allocate static buffers.
    this->lengthReceiveBuffer.resize(4);
    this->lengthSendBuffer.resize(4);
}

void BusConnection::receiveEvent() {
    async_read(*this->socket,
               buffer(&this->lengthReceiveBuffer[0], 4),
               boost::bind(&BusConnection::handleReadLength, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void BusConnection::sendEvent(EventPtr      event,
                              const string &wireSchema) {
    // Serialize the event into a notification object and serialize
    // the notification object.
    protocol::Notification notification;
    fillNotification(notification, event, wireSchema,
                     *static_pointer_cast<string>(event->getData()));
    notification.SerializeToString(&this->messageSendBuffer);

    // Encode the size of the serialized notification object.
    uint32_t length = this->messageSendBuffer.size();
    this->lengthSendBuffer[0] = (length & 0x000000ff) >> 0;
    this->lengthSendBuffer[1] = (length & 0x0000ff00) >> 8;
    this->lengthSendBuffer[2] = (length & 0x00ff0000) >> 16;
    this->lengthSendBuffer[3] = (length & 0xff000000) >> 24;

    // Send the size header, followed by the actual notification data.
    async_write(*this->socket, buffer(this->lengthSendBuffer),
                boost::bind(&BusConnection::handleWriteLength, shared_from_this(),
                     boost::asio::placeholders::error,
                     boost::asio::placeholders::bytes_transferred));
}

void BusConnection::handleReadLength(const system::error_code &error,
                                     size_t                    bytesTransferred) {
    if (error || (bytesTransferred != 4)) {
        RSCWARN(logger, "Receive failure (error " << error << ")"
                << " or incomplete message header (received " << bytesTransferred << ")"
                << "; closing connection");
        this->socket->close();
        this->bus->removeConnection(shared_from_this());
        return;
    }

    uint32_t size
        = ((uint32_t) this->lengthReceiveBuffer[0] << 0)
        | ((uint32_t) this->lengthReceiveBuffer[1] << 8)
        | ((uint32_t) this->lengthReceiveBuffer[2] << 16)
        | ((uint32_t) this->lengthReceiveBuffer[3] << 24);
    this->messageReceiveBuffer.resize(size);

    async_read(*this->socket,
               buffer(&this->messageReceiveBuffer[0], size),
               boost::bind(&BusConnection::handleReadBody, shared_from_this(),
		    boost::asio::placeholders::error,
		    boost::asio::placeholders::bytes_transferred));
}

void BusConnection::handleReadBody(const system::error_code &/*error*/,
                                   size_t                    /*bytesTransferred*/) {
    /** TODO(jmoringe): handle errors */

    // Deserialize the notification.
    this->notification.ParseFromString(this->messageReceiveBuffer);
    EventPtr event(new Event());
    /** TODO(jmoringe): it may be possible to keep a single event
     * instance here since connectors probably have to copy events  */

    // Construct an Event instance *without* deserializing the
    // payload. This has to be done in connectors since different
    // converters can be used.
    MetaData &metaData = event->mutableMetaData();
    metaData.setCreateTime((boost::uint64_t) this->notification.meta_data().create_time());
    metaData.setSendTime((boost::uint64_t) this->notification.meta_data().send_time());
    metaData.setReceiveTime();

    event->setEventId(
            rsc::misc::UUID(
                    (boost::uint8_t*) this->notification.event_id().sender_id().c_str()),
            notification.event_id().sequence_number());
    event->setScope(ScopePtr(new Scope(this->notification.scope())));
    if (this->notification.has_method()) {
        event->setMethod(this->notification.method());
    }

    for (int i = 0; i < this->notification.meta_data().user_infos_size(); ++i) {
        metaData.setUserInfo(this->notification.meta_data().user_infos(i).key(),
                             this->notification.meta_data().user_infos(i).value());
    }
    for (int i = 0; i < this->notification.meta_data().user_times_size(); ++i) {
        metaData.setUserTime(this->notification.meta_data().user_times(i).key(),
                             this->notification.meta_data().user_times(i).timestamp());
    }

    event->setData(boost::shared_ptr<string>(new string(this->notification.data())));
    metaData.setUserInfo("rsb.wire-schema", this->notification.wire_schema());

    // Dispatch the received event to connectors.
    this->bus->handleIncoming(event);

    // Submit task to start receiving the next event.
    receiveEvent();
}

void BusConnection::handleWriteLength(const system::error_code &/*error*/,
                                      size_t                    /*bytesTransferred*/) {
    /** TODO(jmoringe): handle errors */

    async_write(*this->socket, buffer(this->messageSendBuffer),
                boost::bind(&BusConnection::handleWriteBody, shared_from_this(),
                     boost::asio::placeholders::error,
                     boost::asio::placeholders::bytes_transferred));
}

void BusConnection::handleWriteBody(const system::error_code &/*error*/,
                                    size_t                    /*bytesTransferred*/) {
    /** TODO(jmoringe): handle errors */
}

void BusConnection::fillNotification(protocol::Notification &notification,
                                     const EventPtr         &event,
                                     const string           &wireSchema,
                                     const string           &data) {
    // The payload has already been serialized by the connector which
    // submitted the event.
    notification.mutable_event_id()->set_sender_id(
        event->getMetaData().getSenderId().getId().data,
        event->getMetaData().getSenderId().getId().size());
    notification.mutable_event_id()->set_sequence_number(
            event->getEventId().getSequenceNumber());
    notification.set_scope(event->getScope()->toString());
    if (!event->getMethod().empty()) {
        notification.set_method(event->getMethod());
    }
    notification.set_wire_schema(wireSchema);
    notification.mutable_meta_data()->set_create_time(
        event->getMetaData().getCreateTime());
    notification.mutable_meta_data()->set_send_time(
        event->getMetaData().getSendTime());
    for (map<string, string>::const_iterator it =
             event->mutableMetaData().userInfosBegin(); it
             != event->mutableMetaData().userInfosEnd(); ++it) {
        protocol::UserInfo *info =
            notification.mutable_meta_data()->mutable_user_infos()->Add();
        info->set_key(it->first);
        info->set_value(it->second);
    }
    for (map<string, uint64_t>::const_iterator it =
             event->mutableMetaData().userTimesBegin(); it
             != event->mutableMetaData().userTimesEnd(); ++it) {
        protocol::UserTime *info =
            notification.mutable_meta_data()->mutable_user_times()->Add();
        info->set_key(it->first);
        info->set_timestamp(it->second);
    }
    notification.set_num_data_parts(1);
    notification.set_data_part(0);

    notification.set_data(data);
}

void BusConnection::printContents(ostream &stream) const {
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
