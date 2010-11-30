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

#ifndef NOTIFICATION_H_
#define NOTIFICATION_H_

#include <boost/shared_ptr.hpp>

#include "rsb/rsbexports.h"
#include "rsb/protocol/Notification.pb.h"

namespace rsb {
namespace protocol {

// TODO how to export google protocol buffers code? Do we need to export this?
typedef boost::shared_ptr<rsb::protocol::Notification> NotificationPtr;

}
}

#endif /* NOTIFICATION_H_ */
