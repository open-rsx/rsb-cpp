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

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <rsc/runtime/Printable.h>

#include "rsb/rsbexports.h"

namespace rsb {

class Event;
typedef boost::shared_ptr<Event> EventPtr;

namespace transport {
class OutConnector;
typedef boost::shared_ptr<OutConnector> OutConnectorPtr;
}

namespace eventprocessing {

/** Implementations of this interface organize the sending of events
 * via @ref rsb::transport::OutConnector s.
 *
 * @author swrede
 * @author jmoringe
 */
  class RSB_EXPORT EventSendingStrategy: public virtual rsc::runtime::Printable,
                                         private boost::noncopyable {
public:
    virtual ~EventSendingStrategy();

    /**
     * Add @a connector to the list of connectors to which this
     * strategy should deliver events.
     *
     * @param connector The new @ref rsb::transport::OutConnector .
     */
    virtual void addConnector(transport::OutConnectorPtr connector) = 0;

    /**
     * Remove @a connector from the list of connectors to which
     * this strategy should deliver events.
     *
     * @param connector The @ref rsb::transport::OutConnector that
     *                  should be removed.
     */
    virtual void removeConnector(transport::OutConnectorPtr connector) = 0;

    /**
     * Deliver @a event to all @ref rsb::transport::OutConnector
     * objects associated to this strategy.
     *
     * @param event An @ref rsb::Event that should be delivered to
     *              the connectors.
     */
    virtual void process(EventPtr event) = 0;
};

typedef boost::shared_ptr<EventSendingStrategy> EventSendingStrategyPtr;

}
}
