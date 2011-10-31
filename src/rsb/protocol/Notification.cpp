/* ============================================================
 *
 * This file is a part of the RSB project.
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

#include "Notification.h"

#include "../Event.h"
#include "../EventId.h"
#include "../MetaData.h"
#include "../Scope.h"

using namespace std;

namespace rsb {
namespace protocol {

void fillEventId(protocol::EventId &id, const rsb::EventId &realId) {
    boost::uuids::uuid uuid = realId.getParticipantId().getId();
    id.set_sender_id(uuid.data, uuid.size());
    id.set_sequence_number(realId.getSequenceNumber());
}

void fillNotificationId(protocol::Notification &notification,
        const EventPtr &event) {
    fillEventId(*(notification.mutable_event_id()), event->getEventId());
}

void fillNotificationHeader(protocol::Notification &notification,
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
    set<rsb::EventId> causes = event->getCauses();
    for (set<rsb::EventId>::const_iterator causeIt = causes.begin();
            causeIt != causes.end(); ++causeIt) {
        fillEventId(*(notification.add_causes()), *causeIt);
    }

}

}
}
