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

#include <list>
#include <set>
#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/threading/OrderedQueueDispatcherPool.h>

#include "../Event.h"
#include "PushEventReceivingStrategy.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace eventprocessing {

/**
 * This push-style event receiving strategy uses one or more threads
 * to filter @ref rsb::Event s and dispatch matching events to @ref
 * rsb::Handler s.
 *
 * @author jmoringe
 */
class RSB_EXPORT DirectEventReceivingStrategy: public PushEventReceivingStrategy {
public:
    DirectEventReceivingStrategy();

    virtual ~DirectEventReceivingStrategy();

    void printContents(std::ostream &stream) const;

    void setHandlerErrorStrategy(const ParticipantConfig::ErrorStrategy &strategy);

    // Qualification of HandlerPtr is required since there is another
    // HandlerPtr type in eventprocessing.
    virtual void addHandler(rsb::HandlerPtr handler, const bool &wait);
    virtual void removeHandler(rsb::HandlerPtr handler, const bool &wait);

    virtual void addFilter(filter::FilterPtr filter);
    virtual void removeFilter(filter::FilterPtr filter);

    void handle(EventPtr e);

private:
    typedef std::list<rsb::HandlerPtr> HandlerList;

    // Qualification of HandlerPtr is required since there is another
    // HandlerPtr type in eventprocessing.
    bool filter(EventPtr event);
    void deliver(rsb::HandlerPtr handler, EventPtr event);

    void handleDispatchError(const std::string &message);

    rsc::logging::LoggerPtr logger;

    mutable boost::shared_mutex filtersMutex;
    std::set<filter::FilterPtr> filters;

    mutable boost::recursive_mutex errorStrategyMutex;
    ParticipantConfig::ErrorStrategy errorStrategy;

    mutable boost::shared_mutex handlerMutex;
    HandlerList handlers;

    bool singleThreaded;
};

}
}
