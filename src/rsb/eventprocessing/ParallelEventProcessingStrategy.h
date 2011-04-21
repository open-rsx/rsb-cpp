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

#include <set>
#include <utility>

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/threading/OrderedQueueDispatcherPool.h>

#include "../Event.h"
#include "../Subscription.h"
#include "EventProcessingStrategy.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace eventprocessing {

// optimization brainstorming:
// could a req-req condition even be a static map for one process?
// and matching something like [cid]->invoke?!?
// could even be contained in the specific condition class like a ReplyCondition or a TopicCondition
// -> dtm or some other subscription language?!? maybe this is already too much
// router config may then allow very specific and optimized configurations for
// certain patterns using specific condition classes and ports

/**
 * @author swrede
 */
class RSB_EXPORT ParallelEventProcessingStrategy : public EventProcessingStrategy {
public:
	ParallelEventProcessingStrategy();
	// TODO make threadpool size configurable
	ParallelEventProcessingStrategy(unsigned int num_threads);
	virtual ~ParallelEventProcessingStrategy();

	// if invoked, the event is dispatched to listeners, typically called by ports
        void process(rsb::EventPtr e);

        // add a subscription and associated handlers
        void subscribe(rsb::SubscriptionPtr s,
                       std::set<HandlerPtr> handlers);

        // unsubscribe a subscription
        void unsubscribe(rsb::SubscriptionPtr s);
private:
        typedef std::pair< rsb::SubscriptionPtr, std::set<HandlerPtr> > DispatchUnit;
        typedef boost::shared_ptr<DispatchUnit> DispatchUnitPtr;

        bool filter(DispatchUnitPtr dispatch, EventPtr event);
        void deliver(DispatchUnitPtr dispatch, EventPtr event);

        // TODO make list subscriptions
        rsc::logging::LoggerPtr logger;
        rsc::threading::OrderedQueueDispatcherPool<EventPtr, DispatchUnit> pool;

        std::map<SubscriptionPtr, DispatchUnitPtr> dispatchUnitsBySubscription;
};

}
}
