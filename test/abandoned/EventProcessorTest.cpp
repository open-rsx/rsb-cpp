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

#include "rsb/Subscription.h"
#include "rsb/Event.h"
#include "rsb/EventProcessingStrategy.h"
#include "rsb/filter/Filter.h"
#include "rsb/filter/ScopeFilter.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

using namespace std;
using namespace rsb;
using namespace rsb::filter;
using namespace rsb::internal;

void handler(EventPtr e) {
	boost::shared_ptr<string> s = boost::static_pointer_cast<string>(e->getData());
	cout << "Event received: " << *s << endl;
}

int main(void) {
	// create examplary event
	EventPtr e1(new Event("xcf://blah",boost::static_pointer_cast<void>(boost::shared_ptr<string>(new string("Hello World!"))),"EventString"));
	EventPtr e2(new Event("xcf://blahblah",boost::static_pointer_cast<void>(boost::shared_ptr<string>(new string("Hello World!"))),"EventString"));

	// create subscription
	SubscriptionPtr s(new Subscription());
	FilterPtr f(new ScopeFilter("xcf://blah"));
	s->appendFilter(f);

	EventProcessingStrategyPtr ep(new EventProcessingStrategy());

	// register event handler
	Action a = boost::bind(&handler, _1);
	s->appendAction(a);

	// add subscription to ep
	ep->subscribe(s);

	// push events through ep
	// should print hello world
	ep->process(e1);
	// should be filtered
	ep->process(e2);

	return EXIT_SUCCESS;
}
