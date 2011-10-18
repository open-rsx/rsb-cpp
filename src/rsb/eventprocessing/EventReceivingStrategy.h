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
#include <boost/noncopyable.hpp>

#include "../Handler.h"
#include "rsb/rsbexports.h"

namespace rsb {

class Event;
typedef boost::shared_ptr<Event> EventPtr;

namespace filter {
class Filter;
typedef boost::shared_ptr<Filter> FilterPtr;
}

namespace eventprocessing {

/**
 * Implementations of this interface organize the receiving of events
 * via @ref rsb::transport::InConnector s.
 *
 * @author swrede
 * @author jmoringe
 */
class RSB_EXPORT EventReceivingStrategy: public Handler,
                                         private boost::noncopyable {
public:
    virtual ~EventReceivingStrategy();

    virtual void addFilter(filter::FilterPtr filter) = 0;
    virtual void removeFilter(filter::FilterPtr filter) = 0;

    /**
     * Dispatches the event to the listener.
     *
     * @param event the event to dispatch
     */
    virtual void handle(EventPtr event) = 0;
};

typedef boost::shared_ptr<EventReceivingStrategy> EventReceivingStrategyPtr;

}
}
