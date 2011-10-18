/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "InConnector.h"
#include "rsb/rsbexports.h"

namespace rsb {

class Event;
typedef boost::shared_ptr<Event> EventPtr;

namespace transport {

/**
 * Objects of classes which implement this specialized @ref
 * InConnector interface provide the ability to receive events in
 * pull-style manner by means of one transport mechanism.
 *
 * In general, and especially when used in a constellation with one
 * receiving participant and a single @ref InPullConnector, the
 * pull-style data-flow can be much more efficient than the push-style
 * data-flow provided by @ref InPushConnector.
 *
 * @author jmoringe
 */
class RSB_EXPORT InPullConnector: public InConnector {
public:
    typedef boost::shared_ptr<InPullConnector> Ptr;

    virtual ~InPullConnector();

    virtual EventPtr raiseEvent(bool block) = 0;
};

typedef InPullConnector::Ptr InPullConnectorPtr;

}
}
