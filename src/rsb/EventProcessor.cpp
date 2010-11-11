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

#include "EventProcessor.h"

using namespace std;

namespace rsb {

namespace internal {

EventProcessor::EventProcessor() :
	logger(rsc::logging::Logger::getLogger("rsb.EventProcessor")), pool(5,
			boost::bind(&EventProcessor::deliver, this, _1, _2)) {
	pool.start();
}

EventProcessor::EventProcessor(unsigned int num_threads) :
	logger(rsc::logging::Logger::getLogger("rsb.EventProcessor")), pool(
			num_threads, boost::bind(&EventProcessor::deliver, this, _1, _2)) {
	pool.start();
}

EventProcessor::~EventProcessor() {
}

bool EventProcessor::filter(SubscriptionPtr sub, RSBEventPtr e) {
	RSCDEBUG(logger, "Matching event " << e << " for subscription " << sub);

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

void EventProcessor::deliver(SubscriptionPtr sub, RSBEventPtr e) {
	RSCDEBUG(logger, "Delivering event " << e << " for subscription " << sub);

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

void EventProcessor::process(RSBEventPtr e) {
	pool.push(e);
}

void EventProcessor::subscribe(SubscriptionPtr s) {
	pool.registerReceiver(s);
}
void EventProcessor::unsubscribe(SubscriptionPtr s) {
	// TODO subscriptions need to be made thread-safe
	s->disable();
	pool.unregisterReceiver(s);
}

}

}
