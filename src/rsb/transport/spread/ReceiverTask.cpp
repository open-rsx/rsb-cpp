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

using namespace std;
using namespace rsb;
using namespace rsb::protocol;
using namespace rsb::transport;
using namespace rsc::logging;

namespace rsb {
namespace spread {

DataStore::DataStore(rsb::protocol::NotificationPtr n) :
    logger(Logger::getLogger("rsb.spread.DataStore")), receivedParts(0) {
    store.resize(n->num_data_parts() + 1);
    add(n);
}

DataStore::~DataStore() {
}

string DataStore::getData(const unsigned int &i) {
    return store[i]->data().binary();
}

unsigned int DataStore::add(rsb::protocol::NotificationPtr n) {
    RSCTRACE(logger, "Add message " << n->id() << " (part " << n->data_part() << ") to DataStore");
    store[n->data_part()] = n;
    return receivedParts++;
}

ReceiverTask::ReceiverTask(SpreadConnectionPtr s,
        converter::Repository<string>::Ptr converters, HandlerPtr handler) :
    logger(rsc::logging::Logger::getLogger("rsb.spread.ReceiverTask")),
            cancelRequested(false), con(s), converters(converters),
            handler(handler) {

    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    RSCTRACE(logger, "ReceiverTask::RecieverTask, SpreadConnection: " << con);

}

ReceiverTask::~ReceiverTask() {
}

void ReceiverTask::execute() {
    // TODO Do performance optimization for data joining
    // TODO Think about old data in dataPool map

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

    bool multiPartNotification = notification->num_data_parts() > 0;
    if (multiPartNotification) {

        it = dataPool.find(notification->id());
        if (it != dataPool.end()) {

            // Push message to existing DataStore
            RSCTRACE(logger, "Add message to existing data store");
            unsigned int dataStoreSize = it->second->add(notification);

            if (notification->num_data_parts() == dataStoreSize) {
                RSCTRACE(logger, "Join data parts");
                // Concatenate data parts
                completeData.reset(new string(it->second->getData(0)));
                for (unsigned int i = 1; i <= notification->num_data_parts(); ++i) {
                    completeData->append(it->second->getData(i));
                }
                dataPool.erase(it);
            }

        } else {
            // Create new DataStore
            RSCTRACE(logger, "Create new data store for message: " << notification->id());
            dataPool.insert(
                    pair<string, DataStorePtr> (notification->id(),
                            DataStorePtr(new DataStore(notification))));
        }

    } else {
        completeData.reset(new string(notification->data().binary()));
    }

    RSCTRACE(logger, "dataPool size: " << dataPool.size());

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

    // TODO refactor converter handling and conversion
    // TODO error handling
    converter::Converter<string>::Ptr c = converters->getConverterByWireSchema(
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

}
}
