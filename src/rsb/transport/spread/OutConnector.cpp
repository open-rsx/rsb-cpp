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

#include "OutConnector.h"

#include <rsc/misc/langutils.h>

#include "../../protocol/ProtocolException.h"

using namespace std;

using namespace rsc::runtime;
using namespace rsc::logging;

using namespace rsb::protocol;
using namespace rsb::converter;

namespace rsb {
namespace spread {

transport::OutConnector *OutConnector::create(const Properties& args) {
    static LoggerPtr logger = Logger::getLogger("rsb.spread.OutConnector");
    RSCDEBUG(logger, "creating OutConnector with properties " << args);

    return new OutConnector(args.get<UnambiguousConverterMap<string> > (
            "converters"), args.get<string> ("host", defaultHost()),
            args.getAs<unsigned int> ("port", defaultPort()), args.getAs<
                    unsigned int> ("maxfragmentsize", 100000));
}

OutConnector::OutConnector(const UnambiguousConverterMap<string> &converters,
        const string &host, unsigned int port, unsigned int maxFragmentSize) :
    transport::ConverterSelectingConnector<string>(converters), logger(
            Logger::getLogger("rsb.spread.OutConnector")), active(false),
            connector(new SpreadConnector(host, port)), maxFragmentSize(
                    maxFragmentSize) {
}

OutConnector::~OutConnector() {
    if (this->active) {
        deactivate();
    }
}

string OutConnector::getClassName() const {
    return "OutConnector";
}

void OutConnector::printContents(ostream &stream) const {
    stream << "connector = " << connector << ", active = " << active;
}

void OutConnector::activate() {
    this->connector->activate();
    this->active = true;
}

void OutConnector::deactivate() {
    this->connector->deactivate();
    this->active = false;
}

void OutConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec &specs) {
    this->connector->setQualityOfServiceSpecs(specs);
}

void OutConnector::fillNotification(protocol::Notification &notification,
        const EventPtr &event, const string &wireSchema,
        const unsigned int &numDataParts, const unsigned int &dataPart,
        const string &data) {

    notification.set_id(event->getId().getId().data,
            event->getId().getId().size());
    notification.set_scope(event->getScope().toString());
    notification.set_wire_schema(wireSchema);
    notification.mutable_meta_data()->set_sender_id(
            event->getMetaData().getSenderId().getId().data,
            event->getMetaData().getSenderId().getId().size());
    notification.mutable_meta_data()->set_create_time(
            event->getMetaData().getCreateTime());
    notification.mutable_meta_data()->set_send_time(
            event->getMetaData().getSendTime());
    for (map<string, string>::const_iterator it =
            event->mutableMetaData().userInfosBegin(); it
            != event->mutableMetaData().userInfosEnd(); ++it) {
        UserInfo *info =
                notification.mutable_meta_data()->mutable_user_infos()->Add();
        info->set_key(it->first);
        info->set_value(it->second);
    }
    for (map<string, boost::uint64_t>::const_iterator it =
            event->mutableMetaData().userTimesBegin(); it
            != event->mutableMetaData().userTimesEnd(); ++it) {
        UserTime *info =
                notification.mutable_meta_data()->mutable_user_times()->Add();
        info->set_key(it->first);
        info->set_timestamp(it->second);
    }
    notification.set_num_data_parts(numDataParts);
    notification.set_data_part(dataPart);

    notification.set_data(data);

}

void OutConnector::handle(EventPtr event) {

    // TODO Remove "data split" information from notification

    // TODO exception handling if converter is not available
    ConverterPtr c = getConverter(event->getType());
    string wire;
    string wireSchema = c->serialize(make_pair(event->getType(),
            event->getData()), wire);

    // ---- Begin split message implementation ----
    RSCDEBUG(logger, "Whole message size (data only): " << wire.size());
    unsigned int numDataParts = wire.size() / this->maxFragmentSize + 1;
    RSCDEBUG(logger, "Number of message parts (data only): " << numDataParts);

    event->mutableMetaData().setSendTime(rsc::misc::currentTimeMicros());

    size_t curPos = 0;
    for (unsigned int i = 0; i < numDataParts; i++) {

        // extract the data for this message part
        string dataPart;
        if (curPos < (wire.size() - (wire.size() % this->maxFragmentSize))) {
            dataPart = wire.substr(curPos, this->maxFragmentSize);
            curPos = (i * this->maxFragmentSize) + this->maxFragmentSize;
        } else {
            dataPart = wire.substr(curPos, wire.size() % this->maxFragmentSize);
        }
        RSCTRACE(logger, "Size of message[" << i << "] (data only): " << dataPart.size());

        // create a notification
        Notification notification;
        fillNotification(notification, event, wireSchema, numDataParts, i,
                dataPart);

        // serialize to spread message
        string serializedMessageData;
        if (!notification.SerializeToString(&serializedMessageData)) {
            throw ProtocolException("Failed to write notification to stream");
        }
        SpreadMessage spreadMessage(serializedMessageData);

        // send message to appropriate groups
        vector<Scope> sendScopes = event->getScope().superScopes(true);
        for (vector<Scope>::const_iterator scopeIt = sendScopes.begin(); scopeIt
                != sendScopes.end(); ++scopeIt) {
            spreadMessage.addGroup(connector->makeGroupName(*scopeIt));
        }
        spreadMessage.setQOS(this->connector->getMessageQoS());

        RSCTRACE(logger, "This is the serialized message size before send: " << spreadMessage.getSize());

        if (!this->connector->send(spreadMessage)) {
            // TODO implement queing or throw messages away?
            // TODO maybe return exception with msg that was not sent
            // TODO especially important to fulfill QoS specs
            RSCWARN(logger, "Spread Connection inactive -> could not send message");
        } else {
            RSCDEBUG(logger, "event sent to spread");
        }

    }

}

}
}
