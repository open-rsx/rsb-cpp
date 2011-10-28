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

#include "Notification.h"
#include "rsb/rsbexports.h"
#include "rsb/protocol/FragmentedNotification.pb.h"

namespace rsb {
namespace protocol {

typedef boost::shared_ptr<rsb::protocol::FragmentedNotification> FragmentedNotificationPtr;

/**
 * A deleter objects for boost::shared_ptr which makes it possible to expose
 * the Notification contained in a FragmentedNotification as a shared_ptr.
 *
 * @author jwienke
 */
class NotificationDeleter {
public:
    NotificationDeleter(FragmentedNotificationPtr fragment);
    void operator()(void const *);
private:
    FragmentedNotificationPtr fragment;
};

}
}

