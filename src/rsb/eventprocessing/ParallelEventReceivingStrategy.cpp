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

#include "ParallelEventReceivingStrategy.h"

using namespace std;

namespace rsb {
namespace eventprocessing {

ParallelEventReceivingStrategy::ParallelEventReceivingStrategy() :
	logger(rsc::logging::Logger::getLogger("rsb.ParallelEventReceivingStrategy")),
	pool(5,
	     boost::bind(&ParallelEventReceivingStrategy::deliver, this, _1, _2),
	     boost::bind(&ParallelEventReceivingStrategy::filter,  this, _1, _2)) {
	pool.start();
}

ParallelEventReceivingStrategy::ParallelEventReceivingStrategy(unsigned int num_threads) :
	logger(rsc::logging::Logger::getLogger("rsb.ParallelEventReceivingStrategy")),
        pool(num_threads,
             boost::bind(&ParallelEventReceivingStrategy::deliver, this, _1, _2),
             boost::bind(&ParallelEventReceivingStrategy::filter,  this, _1, _2)) {
	pool.start();
}

ParallelEventReceivingStrategy::~ParallelEventReceivingStrategy() {
	pool.stop();
}

bool ParallelEventReceivingStrategy::filter(DispatchUnitPtr dispatch, EventPtr e) {
	RSCDEBUG(logger, "Matching event " << *e << " for subscription " << *dispatch->first);

	if (!dispatch->first->isEnabled()) {
		return false;
	}

	bool match = false;
	// match event
	try {
		match = dispatch->first->match(e);
	} catch (const exception& ex) {
		// TODO probably disable this subscription
		RSCFATAL(logger, "Exception matching event " << *e
				<< " for subscription " << *dispatch->first << ":" << ex.what());
	} catch (...) {
		RSCFATAL(logger, "Catch-all exception matching event " << *e
				<< " for subscription " << *dispatch->first);
	}

	return match;

}

void ParallelEventReceivingStrategy::deliver(DispatchUnitPtr dispatch, EventPtr e) {
	RSCDEBUG(logger, "Delivering event " << *e << " for subscription " << *dispatch->first);

	if (!dispatch->first->isEnabled()) {
		return;
	}

	try {

		const set<HandlerPtr>& handlers = dispatch->second;
		RSCTRACE(logger, "Match and subscriber is enabled, dispatching to "
				<< handlers.size() << " handlers");
		for (set<HandlerPtr>::const_iterator handlerIt = handlers.begin(); handlerIt
				!= handlers.end(); ++handlerIt) {
			(*handlerIt)->handle(e);
		}

	} catch (const exception& ex) {
		// TODO probably disable this subscription
		RSCFATAL(logger, "Exception delivering event " << *e
                         << " to subscription " << *dispatch->first << ":" << ex.what());
	} catch (...) {
		RSCFATAL(logger, "Catch-all exception delivering event " << *e
                         << " to subscription " << *dispatch->first);
	}

}

void ParallelEventReceivingStrategy::process(EventPtr e) {
	pool.push(e);
}

void ParallelEventReceivingStrategy::subscribe(SubscriptionPtr s,
                                        set<HandlerPtr> handlers) {
	pool.registerReceiver(DispatchUnitPtr(new DispatchUnit(s, handlers)));
}
void ParallelEventReceivingStrategy::unsubscribe(SubscriptionPtr s) {
	// TODO subscriptions need to be made thread-safe
	s->disable();
        assert(dispatchUnitsBySubscription.find(s) != dispatchUnitsBySubscription.end());
	pool.unregisterReceiver(dispatchUnitsBySubscription[s]);
}

}
}
