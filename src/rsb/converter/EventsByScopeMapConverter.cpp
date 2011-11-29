/* ============================================================
 *
 * This file is a part of the RSB TimeSync project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "EventsByScopeMapConverter.h"

#include <map>
#include <vector>

#include "../Event.h"
#include "../EventId.h"
#include "../EventCollections.h"
#include "../MetaData.h"
#include "../Scope.h"
#include "../protocol/Notification.h"
#include "../protocol/collections/EventsByScopeMap.pb.h"
#include "Converter.h"
#include "ProtocolBufferConverter.h"
#include "SerializationException.h"

using namespace std;

namespace rsb {
namespace converter {

EventsByScopeMapConverter::EventsByScopeMapConverter(
        ConverterSelectionStrategy<std::string>::Ptr serializationConverters,
        ConverterSelectionStrategy<std::string>::Ptr deserializationConverters) :
        Converter<string>("dummy", RSB_TYPE_TAG(EventsByScopeMap)), serializationConverters(
                serializationConverters), deserializationConverters(
                deserializationConverters), converter(
                new ProtocolBufferConverter<
                        protocol::collections::EventsByScopeMap>) {

}

EventsByScopeMapConverter::~EventsByScopeMapConverter() {
}

string EventsByScopeMapConverter::getWireSchema() const {
    return converter->getWireSchema();
}

string EventsByScopeMapConverter::getClassName() const {
    return "EventsByScopeMapConverter";
}

void fillEventId(protocol::EventId& id, const rsb::EventId& realId) {
    boost::uuids::uuid uuid = realId.getParticipantId().getId();
    id.set_sender_id(uuid.data, uuid.size());
    id.set_sequence_number(realId.getSequenceNumber());
}

void fillNotificationId(protocol::Notification& notification,
        const EventPtr& event) {
    fillEventId(*(notification.mutable_event_id()), event->getEventId());
}

void fillNotificationHeader(protocol::Notification& notification,
        const EventPtr& event, const string& wireSchema) {

    notification.set_wire_schema(wireSchema);

    notification.set_scope(event->getScopePtr()->toString());
    if (!event->getMethod().empty()) {
        notification.set_method(event->getMethod());
    }
    notification.mutable_meta_data()->set_create_time(
            event->getMetaData().getCreateTime());
    notification.mutable_meta_data()->set_send_time(
            event->getMetaData().getSendTime());
    notification.mutable_meta_data()->set_receive_time(
            event->getMetaData().getReceiveTime());
    notification.mutable_meta_data()->set_deliver_time(
            event->getMetaData().getDeliverTime());
    for (map<string, string>::const_iterator it =
            event->mutableMetaData().userInfosBegin();
            it != event->mutableMetaData().userInfosEnd(); ++it) {
        protocol::UserInfo* info =
                notification.mutable_meta_data()->mutable_user_infos()->Add();
        info->set_key(it->first);
        info->set_value(it->second);
    }
    for (map<string, boost::uint64_t>::const_iterator it =
            event->mutableMetaData().userTimesBegin();
            it != event->mutableMetaData().userTimesEnd(); ++it) {
        protocol::UserTime* info =
                notification.mutable_meta_data()->mutable_user_times()->Add();
        info->set_key(it->first);
        info->set_timestamp(it->second);
    }
    set<rsb::EventId> causes = event->getCauses();
    for (set<rsb::EventId>::const_iterator causeIt = causes.begin();
            causeIt != causes.end(); ++causeIt) {
        fillEventId(*(notification.add_causes()), *causeIt);
    }

}

void fillEvent(EventPtr event, const protocol::Notification& notification,
        VoidPtr data, const string& dataType) {

    event->mutableMetaData().setCreateTime(
            (boost::uint64_t) notification.meta_data().create_time());
    event->mutableMetaData().setSendTime(
            (boost::uint64_t) notification.meta_data().send_time());
    event->mutableMetaData().setReceiveTime(
            (boost::uint64_t) notification.meta_data().receive_time());
    event->mutableMetaData().setDeliverTime(
            (boost::uint64_t) notification.meta_data().deliver_time());
    event->setEventId(
            rsc::misc::UUID(
                    (boost::uint8_t*) notification.event_id().sender_id().c_str()),
            notification.event_id().sequence_number());

    event->setScopePtr(ScopePtr(new Scope(notification.scope())));
    if (notification.has_method()) {
        event->setMethod(notification.method());
    }

    for (int i = 0; i < notification.meta_data().user_infos_size(); ++i) {
        event->mutableMetaData().setUserInfo(
                notification.meta_data().user_infos(i).key(),
                notification.meta_data().user_infos(i).value());
    }
    for (int i = 0; i < notification.meta_data().user_times_size(); ++i) {
        event->mutableMetaData().setUserTime(
                notification.meta_data().user_times(i).key(),
                notification.meta_data().user_times(i).timestamp());
    }
    for (int i = 0; i < notification.causes_size(); ++i) {
        protocol::EventId cause = notification.causes(i);
        event->addCause(
                rsb::EventId(
                        rsc::misc::UUID(
                                (boost::uint8_t*) cause.sender_id().c_str()),
                        cause.sequence_number()));
    }

    event->setType(dataType);
    event->setData(data);

}

string EventsByScopeMapConverter::serialize(const AnnotatedData& data,
        string& wire) {

    if (data.first != getDataType()) {
        throw SerializationException(
                "Called with unsupported data type " + data.first);
    }

    boost::shared_ptr<EventsByScopeMap> dataMap = boost::static_pointer_cast<
            EventsByScopeMap>(data.second);

    boost::shared_ptr<protocol::collections::EventsByScopeMap> syncMap(
            new protocol::collections::EventsByScopeMap);

    // iterate over all scopes
    for (EventsByScopeMap::const_iterator mapIt = dataMap->begin();
            mapIt != dataMap->end(); ++mapIt) {

        protocol::collections::EventsByScopeMap::ScopeSet* scopeSet =
                syncMap->add_sets();
        scopeSet->set_scope(mapIt->first.toString());

        // iterate over all events in one scope
        for (vector<EventPtr>::const_iterator eventIt = mapIt->second.begin();
                eventIt != mapIt->second.end(); ++eventIt) {

            // convert event to notification
            EventPtr event = *eventIt;

            Converter<string>::Ptr c = serializationConverters->getConverter(
                    event->getType());
            string wire;
            string wireSchema = c->serialize(
                    make_pair(event->getType(), event->getData()), wire);

            protocol::Notification* notification =
                    scopeSet->add_notifications();
            fillNotificationId(*notification, event);
            fillNotificationHeader(*notification, event, wireSchema);
            notification->set_data(wire);

        }

    }

    return converter->serialize(
            make_pair(
                    rsc::runtime::typeName<
                            protocol::collections::EventsByScopeMap>(),
                    syncMap), wire);

}

AnnotatedData EventsByScopeMapConverter::deserialize(const string& wireSchema,
        const string& wire) {

    if (wireSchema != getWireSchema()) {
        throw SerializationException("Unexpected wire schema " + wireSchema);
    }

    protocol::collections::EventsByScopeMap syncMap;
    syncMap.ParseFromString(wire);

    boost::shared_ptr<EventsByScopeMap> dataMap(new EventsByScopeMap);

    // iterate over all scope sets
    for (int setCount = 0; setCount < syncMap.sets_size(); ++setCount) {

        const protocol::collections::EventsByScopeMap::ScopeSet& scopeSet =
                syncMap.sets(setCount);
        ScopePtr scope(new Scope(scopeSet.scope()));

        // iterate over all notifications in each scope set
        for (int notificationIndex = 0;
                notificationIndex < scopeSet.notifications_size();
                ++notificationIndex) {

            // convert the notification back to an event
            const protocol::Notification& notification = scopeSet.notifications(
                    notificationIndex);

            EventPtr event(new Event);
            event->setScopePtr(scope);
            AnnotatedData annotatedData =
                    deserializationConverters->getConverter(
                            notification.wire_schema())->deserialize(
                            notification.wire_schema(), notification.data());
            event->setType(annotatedData.first);
            event->setData(annotatedData.second);

            fillEvent(event, notification, annotatedData.second,
                    annotatedData.first);

            (*dataMap)[*scope].push_back(event);

        }

    }

    return make_pair(getDataType(), dataMap);

}

}
}
