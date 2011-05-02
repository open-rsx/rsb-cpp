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

#include "../Event.h"
#include "Handler.h"
#include "../filter/Filter.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace eventprocessing {

/**
 * Implementations of this interface organize the receiving of events
 * via @ref rsb::transport::InConnector s.
 *
 * @author swrede
 * @author jmoringe
 */
class RSB_EXPORT EventReceivingStrategy: public Handler {
public:

    virtual ~EventReceivingStrategy();

    virtual void addHandler(HandlerPtr handler) = 0;
    virtual void removeHandler(HandlerPtr handler) = 0;

    virtual void addFilter(filter::FilterPtr filter) = 0;
    virtual void removeFilter(filter::FilterPtr filter) = 0;

    // if invoked, the event is dispatched to listeners, typically called by ports
    virtual void handle(rsb::EventPtr event) = 0;
};

typedef boost::shared_ptr<EventReceivingStrategy> EventReceivingStrategyPtr;

}
}
