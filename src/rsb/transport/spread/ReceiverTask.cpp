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

        // TODO think about how to deal with non-data messages, e.g., membership
        if (message->getType() != SpreadMessage::REGULAR) {
            return;
        }

        NotificationPtr notification(new Notification());
        if (!notification->ParseFromString(message->getDataAsString())) {
            throw CommException("Failed to parse notification in pbuf format");
        }
        RSCTRACE(logger, "Parsed event ID: " << notification->id());
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
        if (!isCancelRequested()) {
            RSCERROR(logger, "Error receiving spread message: " << e.what());
        } else {
            // safely ignore, invalid mbox just signals in this context
            // that the connection to spread was deactivated
        }
    }

}

boost::shared_ptr<string> ReceiverTask::handleAndJoinNotification(
        NotificationPtr notification) {

    boost::shared_ptr<string> completeData;

    bool multiPartNotification = notification->num_data_parts() > 1;
    if (multiPartNotification) {
        completeData = this->assemblyPool->add(notification);
    } else {
        completeData.reset(new string(notification->data().binary()));
    }
    return completeData;

}

void ReceiverTask::notifyHandler(NotificationPtr notification,
        boost::shared_ptr<string> data) {

    EventPtr e(new Event());

    e->setId(rsc::misc::UUID(notification->id()));
    e->setScope(Scope(notification->scope()));

    for (int i = 0; i < notification->meta_infos_size(); ++i) {
        e->addMetaInfo(notification->meta_infos(i).key(),
                notification->meta_infos(i).value());
    }

    // TODO error handling
    InConnector::ConverterPtr c = this->connector->getConverter(
            notification->wire_schema());
    converter::AnnotatedData deserialized = c->deserialize(
            notification->wire_schema(), *data);
    e->setType(deserialized.first);
    e->setData(deserialized.second);

    if (this->handler) {
        this->handler->handle(e);
    }

}

void ReceiverTask::setHandler(HandlerPtr handler) {
    this->handler = handler;
}

void ReceiverTask::setPruning(const bool &pruning) {
    assemblyPool->setPruning(pruning);
}

}
}
