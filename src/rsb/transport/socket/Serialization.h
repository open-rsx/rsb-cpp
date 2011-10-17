/** ============================================================
 *
 * This file is part of the RSB project.
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * ============================================================  */

#pragma once

#include "../../Event.h"
#include "../../protocol/Notification.h"

namespace rsb {
namespace transport {
namespace socket {
/**
 * Converts @a notification into an @ref Event. The event payload will
 * be copied from @a notification into the event unmodified to allow
 * application of arbitrary converters.
 *
 * @param notification The @ref protocol::Notification from which the
 *                     event should be constructed.
 * @param exposeWireSchema Controls whether the wire-schema stored in
 *                         @a notification should be exposed in a meta
 *                         data item of the created event.
 * @return A shared pointer to a newly allocated @ref rsb::Event.
 */
EventPtr notificationToEvent(protocol::Notification &notification,
                             bool                    exposeWireSchema = false);
/**
 * Converts the @ref Event @a event into a @ref
 * protocol::Notification, storing the result in @a notification.
 *
 * @param notification The @ref protocol::Notification object into
 *                     which the conversion should be performed.
 * @param event event The @ref Event object that should be serialized.
 * @param wireSchema The wire-Schema that should be stored in @a
 *                   notification.
 * @param data The payload that should be stored in @a notification.
 */
void eventToNotification(protocol::Notification &notification,
                         const EventPtr         &event,
                         const std::string      &wireSchema,
                         const std::string      &data);

}
}
}
