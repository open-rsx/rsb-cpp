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

#include "EventProcessingStrategy.h"

using namespace std;

namespace rsb {
namespace eventprocessing {

EventProcessingStrategy::EventProcessingStrategy() :
	logger(rsc::logging::Logger::getLogger("rsb.EventProcessingStrategy")), pool(5,
			boost::bind(&EventProcessingStrategy::deliver, this, _1, _2), boost::bind(
					&EventProcessingStrategy::filter, this, _1, _2)) {
	pool.start();
}

EventProcessingStrategy::EventProcessingStrategy(unsigned int num_threads) :
	logger(rsc::logging::Logger::getLogger("rsb.EventProcessingStrategy")), pool(
			num_threads, boost::bind(&EventProcessingStrategy::deliver, this, _1, _2),
			boost::bind(&EventProcessingStrategy::filter, this, _1, _2)) {
	pool.start();
}

EventProcessingStrategy::~EventProcessingStrategy() {
}

bool EventProcessingStrategy::filter(SubscriptionPtr sub, EventPtr e) {
	RSCDEBUG(logger, "Matching event " << *e << " for subscription " << *sub);

	if (!sub->isEnabled()) {
		return false;
	}

	bool match = false;
	// match event
	try {
		match = sub->match(e);
	} catch (const exception& ex) {
		// TODO probably disable this subscription
		RSCFATAL(logger, "Exception matching event " << *e
				<< " for subscription " << *sub << ":" << ex.what());
	} catch (...) {
		RSCFATAL(logger, "Catch-all exception matching event " << *e
				<< " for subscription " << *sub);
	}

	return match;

}

void EventProcessingStrategy::deliver(SubscriptionPtr sub, EventPtr e) {
	RSCDEBUG(logger, "Delivering event " << *e << " for subscription " << *sub);

	if (!sub->isEnabled()) {
		return;
	}

	try {

		boost::shared_ptr<set<HandlerPtr> > handlers = sub->getHandlers();
		RSCTRACE(logger, "Match and subscriber is enabled, dispatching to "
				<< handlers->size() << " handlers");
		for (set<HandlerPtr>::iterator handlerIt = handlers->begin(); handlerIt
				!= handlers->end(); ++handlerIt) {
			(*handlerIt)->internal_notify(e);
		}

	} catch (const exception& ex) {
		// TODO probably disable this subscription
		RSCFATAL(logger, "Exception delivering event " << *e
				<< " to subscription " << *sub << ":" << ex.what());
	} catch (...) {
		RSCFATAL(logger, "Catch-all exception delivering event " << *e
				<< " to subscription " << *sub);
	}

}

void EventProcessingStrategy::process(EventPtr e) {
	pool.push(e);
}

void EventProcessingStrategy::subscribe(SubscriptionPtr s) {
	pool.registerReceiver(s);
}
void EventProcessingStrategy::unsubscribe(SubscriptionPtr s) {
	// TODO subscriptions need to be made thread-safe
	s->disable();
	pool.unregisterReceiver(s);
}

}
}
