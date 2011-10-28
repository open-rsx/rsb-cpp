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
#include "../../MetaData.h"
#include "../../EventId.h"
#include "../../Scope.h"

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

    return new OutConnector(
            args.get<ConverterSelectionStrategyPtr>("converters"),
            args.get<string>("host", defaultHost()),
            args.getAs<unsigned int>("port", defaultPort()),
            args.getAs<unsigned int>("maxfragmentsize", 100000));
}

OutConnector::OutConnector(ConverterSelectionStrategyPtr converters,
        const string &host, unsigned int port, unsigned int maxFragmentSize) :
        transport::ConverterSelectingConnector<string>(converters), logger(
                Logger::getLogger("rsb.spread.OutConnector")), active(false), connector(
                new SpreadConnector(host, port)), maxFragmentSize(
                maxFragmentSize), minDataSpace(5) {
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

void OutConnector::fillEventId(protocol::EventId &id, const EventId &realId) {
    boost::uuids::uuid uuid = realId.getParticipantId().getId();
    id.set_sender_id(uuid.data, uuid.size());
    id.set_sequence_number(realId.getSequenceNumber());
}

void OutConnector::fillMandatoryNotificationFields(
        protocol::Notification &notification, const EventPtr &event) {
    fillEventId(*(notification.mutable_event_id()), event->getEventId());
}

void OutConnector::fillNotificationHeader(protocol::Notification &notification,
        const EventPtr &event, const string &wireSchema) {

    notification.set_wire_schema(wireSchema);

    notification.set_scope(event->getScopePtr()->toString());
    if (!event->getMethod().empty()) {
        notification.set_method(event->getMethod());
    }
    notification.mutable_meta_data()->set_create_time(
            event->getMetaData().getCreateTime());
    notification.mutable_meta_data()->set_send_time(
            event->getMetaData().getSendTime());
    for (map<string, string>::const_iterator it =
            event->mutableMetaData().userInfosBegin();
            it != event->mutableMetaData().userInfosEnd(); ++it) {
        UserInfo *info =
                notification.mutable_meta_data()->mutable_user_infos()->Add();
        info->set_key(it->first);
        info->set_value(it->second);
    }
    for (map<string, boost::uint64_t>::const_iterator it =
            event->mutableMetaData().userTimesBegin();
            it != event->mutableMetaData().userTimesEnd(); ++it) {
        UserTime *info =
                notification.mutable_meta_data()->mutable_user_times()->Add();
        info->set_key(it->first);
        info->set_timestamp(it->second);
    }
    set<EventId> causes = event->getCauses();
    for (set<EventId>::const_iterator causeIt = causes.begin();
            causeIt != causes.end(); ++causeIt) {
        fillEventId(*(notification.add_causes()), *causeIt);
    }

}

void OutConnector::handle(EventPtr event) {

    // TODO factor out several methods

    // TODO exception handling if converter is not available
    ConverterPtr c = getConverter(event->getType());
    string wire;
    string wireSchema = c->serialize(
            make_pair(event->getType(), event->getData()), wire);

    event->mutableMetaData().setSendTime(rsc::misc::currentTimeMicros());

    // create a list of all fragments required to send this event
    vector<FragmentedNotificationPtr> fragments;

    size_t curPos = 0;
    unsigned int currentDataPart = 0;
    while (curPos <= wire.size()) {

        FragmentedNotificationPtr notification(new FragmentedNotification);
        fillMandatoryNotificationFields(*(notification->mutable_notification()),
                event);

        // when sending the first time, we need to transmit all meta data.
        if (curPos == 0) {
            fillNotificationHeader(*(notification->mutable_notification()),
                    event, wireSchema);
        }
        // Scale the data for this message with the size of the generated header
        // and mandatory id fields in the notification
        unsigned int headerByteSize = notification->ByteSize();
        assert(headerByteSize <= maxFragmentSize - minDataSpace);
        if (headerByteSize >= maxFragmentSize - minDataSpace) {
            throw ProtocolException(
                    "The meta data of this event are too big for spread!");
        }
        unsigned int maxDataPartSize = maxFragmentSize - headerByteSize;

        // finally set the data information
        string dataPart = wire.substr(curPos, maxDataPartSize);
        curPos += maxDataPartSize;

        notification->mutable_notification()->set_data(dataPart);
        notification->set_data_part(currentDataPart);
        // optimistic guess for the number of required fragments
        notification->set_num_data_parts(1);

        fragments.push_back(notification);

        ++currentDataPart;

    }

    // adapt num_data_parts field of each FragmentedNotification if we need more
    // than one fragment
    assert(!fragments.empty());
    if (fragments.size() > 1) {
        for (vector<FragmentedNotificationPtr>::iterator fragmentIt =
                fragments.begin(); fragmentIt != fragments.end();
                ++fragmentIt) {
            (*fragmentIt)->set_num_data_parts(fragments.size());
        }
    }

    // finally send all fragments

    for (vector<FragmentedNotificationPtr>::const_iterator fragmentIt =
            fragments.begin(); fragmentIt != fragments.end(); ++fragmentIt) {

        // serialize to spread message
        string serializedMessageData;
        if (!(*fragmentIt)->SerializeToString(&serializedMessageData)) {
            throw ProtocolException("Failed to write notification to stream");
        }
        SpreadMessage spreadMessage(serializedMessageData);

        // send message to appropriate groups
        const vector<string> &groupNames = connector->makeGroupNames(
                *event->getScopePtr());
        for (vector<string>::const_iterator groupIt = groupNames.begin();
                groupIt != groupNames.end(); ++groupIt) {
            spreadMessage.addGroup(*groupIt);
        }
        spreadMessage.setQOS(this->connector->getMessageQoS());

        RSCTRACE(
                logger,
                "This is the serialized message size before send: " << spreadMessage.getSize());

        this->connector->send(spreadMessage);
        // TODO implement queuing or throw messages away?
        // TODO maybe return exception with msg that was not sent
        // TODO especially important to fulfill QoS specs
        RSCDEBUG(logger, "event sent to spread");

    }

}

}
}
