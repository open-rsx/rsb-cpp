/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include "ReceiverTask.h"

#include <rsc/misc/langutils.h>
#include <rsc/debug/DebugTools.h>

#include "../../converter/Converter.h"
#include "../../CommException.h"
#include "SpreadConnection.h"
#include "InConnector.h"

using namespace std;
using namespace rsb;
using namespace rsb::protocol;
using namespace rsb::transport;
using namespace rsb::eventprocessing;
using namespace rsc::logging;

namespace rsb {
namespace spread {

ReceiverTask::ReceiverTask(SpreadConnectionPtr s, HandlerPtr handler,
        InConnector* connector) :
    logger(rsc::logging::Logger::getLogger("rsb.spread.ReceiverTask")),
            cancelRequested(false), con(s), connector(connector), assemblyPool(
                    new AssemblyPool()), handler(handler) {

    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    RSCTRACE(logger, "ReceiverTask::RecieverTask, SpreadConnection: " << con);

}

ReceiverTask::~ReceiverTask() {
}

void ReceiverTask::execute() {
    // TODO Do performance optimization for data joining
    try {

        SpreadMessagePtr message(new SpreadMessage(SpreadMessage::REGULAR));
        con->receive(message);
        if (!message) {
            throw CommException(
                    "Receiving a SpreadMessage returned a zero pointer, why?");
        }

        RSCDEBUG(logger, "ReceiverTask::execute new SpreadMessage received " << message);

        if (message->getType() != SpreadMessage::REGULAR) {
            return;
        }

        NotificationPtr notification(new Notification());
        if (!notification->ParseFromString(message->getDataAsString())) {
            throw CommException("Failed to parse notification in pbuf format");
        }
        RSCTRACE(logger, "Parsed event seqnum: " << notification->event_id().sequence_number());
        RSCTRACE(logger, "Binary length: " << notification->data().length());
        RSCTRACE(logger, "Number of split message parts: " << notification->num_data_parts());
        RSCTRACE(logger, "... received message part    : " << notification->data_part());

        // Build data from parts
        boost::shared_ptr<string> completeData = handleAndJoinNotification(
                notification);
        if (completeData) {
            notifyHandler(notification, completeData);
        }

    } catch (rsb::CommException &e) {
        // TODO QoS would not like swallowing the exception
        // TODO maybe at least use the ErrorHandlingStrategy here?
        rsc::debug::DebugToolsPtr tools = rsc::debug::DebugTools::newInstance();
        RSCERROR(logger, "Error receiving spread message: " << e.what() << endl << tools->exceptionInfo(e));
    } catch (boost::thread_interrupted &e) {
        return;
    }

}

boost::shared_ptr<string> ReceiverTask::handleAndJoinNotification(
        NotificationPtr notification) {

    boost::shared_ptr<string> completeData;

    bool multiPartNotification = notification->num_data_parts() > 1;
    if (multiPartNotification) {
        completeData = this->assemblyPool->add(notification);
    } else {
        completeData.reset(new string(notification->data()));
    }
    return completeData;

}

void ReceiverTask::notifyHandler(NotificationPtr notification,
        boost::shared_ptr<string> data) {

    EventPtr e(new Event());

    e->mutableMetaData().setCreateTime((boost::uint64_t) notification->meta_data().create_time());
    e->mutableMetaData().setSendTime((boost::uint64_t) notification->meta_data().send_time());
    e->mutableMetaData().setReceiveTime();
    e->setEventId(rsc::misc::UUID(
            (boost::uint8_t*) notification->event_id().sender_id().c_str()), notification->event_id().sequence_number());

    e->setScope(ScopePtr(new Scope(notification->scope())));
    if (notification->has_method()) {
        e->setMethod(notification->method());
    }

    for (int i = 0; i < notification->meta_data().user_infos_size(); ++i) {
        e->mutableMetaData().setUserInfo(
                notification->meta_data().user_infos(i).key(),
                notification->meta_data().user_infos(i).value());
    }
    for (int i = 0; i < notification->meta_data().user_times_size(); ++i) {
        e->mutableMetaData().setUserTime(
                notification->meta_data().user_times(i).key(),
                notification->meta_data().user_times(i).timestamp());
    }
    for (int i = 0; i < notification->causes_size(); ++i) {
        protocol::EventId cause = notification->causes(i);
        e->addCause(
                EventId(
                        rsc::misc::UUID(
                                (boost::uint8_t*) cause.sender_id().c_str()),
                        cause.sequence_number()));
    }

    // TODO error handling
    InConnector::ConverterPtr c = this->connector->getConverter(
            notification->wire_schema());
    converter::AnnotatedData deserialized = c->deserialize(
            notification->wire_schema(), *data);
    e->setType(deserialized.first);
    e->setData(deserialized.second);

    boost::recursive_mutex::scoped_lock lock(handlerMutex);
    if (this->handler) {
        this->handler->handle(e);
    }

}

void ReceiverTask::setHandler(HandlerPtr handler) {
    boost::recursive_mutex::scoped_lock lock(handlerMutex);
    this->handler = handler;
}

void ReceiverTask::setPruning(const bool &pruning) {
    assemblyPool->setPruning(pruning);
}

}
}
