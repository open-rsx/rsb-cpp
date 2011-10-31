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

#pragma once

#include <boost/shared_ptr.hpp>

#include "rsb/rsbexports.h"
#include "rsb/protocol/Notification.pb.h"

namespace rsb {

class EventId;

class Event;
typedef boost::shared_ptr<Event> EventPtr;

namespace protocol {

typedef boost::shared_ptr<rsb::protocol::Notification> NotificationPtr;

/**
 * Fills the notification with minimal information required to identify it
 * along several notifications sent on a wire. Namely, this is the EventId
 * contained in the notification.
 *
 * @param notification the notification to fill
 * @param event the event to take then information from
 */
RSB_EXPORT void fillNotificationId(protocol::Notification &notification,
        const EventPtr &event);

/**
 * Fills a protocol::Notification with header contents. This includes meta
 * data and causal vector but excludes the id information. These can be set
 * with #fillNotificationId.
 *
 * @param notification notification to fill
 * @param event the event that generated the notification
 * @param wireSchema wire schema of the serialized data
 */
RSB_EXPORT void fillNotificationHeader(protocol::Notification &notification,
        const EventPtr &event, const std::string &wireSchema);

/**
 * Utility function to fill a protocol::EventId.
 *
 * @param id the id to fill
 * @param realId the source id
 */
RSB_EXPORT void fillEventId(protocol::EventId &id, const rsb::EventId &realId);

}
}

