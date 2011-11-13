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

#include "EventReceivingStrategy.h"
#include "rsb/rsbexports.h"

namespace rsb {

class ParticipantConfig;

namespace eventprocessing {

/**
 * This class serves as a base class for event receiving strategy
 * classes that deliver @ref rsb::Event s to @ref rsb::Handler s
 * without triggering by the receiving object. To achieve this, a list
 * of handlers is maintained and dispatching of events is done by
 * calling each handler.
 *
 * @author jmoringe
 */
class RSB_EXPORT PushEventReceivingStrategy: public EventReceivingStrategy {
public:
    /**
     * Defines the strategy to use for handling dispatching errors to the client
     * handler.
     *
     * @param strategy the new strategy to use
     */
    virtual void setHandlerErrorStrategy(
        const ParticipantConfig::ErrorStrategy& strategy) = 0;

    /**
     * Adds a new handler that will be notified about new events.
     *
     * @param handler handler to add
     * @param wait if set to @c true, this method must only return after the
     *             handler has been install completely so that the next event
     *             will be delivered to it
     */
    // Qualification of HandlerPtr is required since there is another
    // HandlerPtr type in eventprocessing.
    virtual void addHandler(rsb::HandlerPtr handler, const bool& wait) = 0;

    /**
     * Removes a handler that will will then not be notified anymore.
     *
     * @param handler handler to remove
     * @param wait if set to @c true, this method must only return after the
     *             handler has been removed completely and will not receive
     *             any more notifications
     */
    virtual void removeHandler(rsb::HandlerPtr handler, const bool& wait) = 0;
};

typedef boost::shared_ptr<PushEventReceivingStrategy> PushEventReceivingStrategyPtr;

}
}
