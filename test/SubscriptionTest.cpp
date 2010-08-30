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
#include "rsb/RSBEvent.h"
#include "rsb/filter/AbstractFilter.h"
#include "rsb/filter/ScopeFilter.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

using namespace std;
using namespace rsb;
using namespace rsb::filter;

void handler(RSBEventPtr e) {
	boost::shared_ptr<string> s = boost::static_pointer_cast<string>(e->getData());
	cout << "Event received: " << *s << endl;
}

int main(void) {
	// create examplary event
	RSBEventPtr e(new RSBEvent("xcf://blah",boost::static_pointer_cast<void>(boost::shared_ptr<string>(new string("Hello World!"))),"RSBEventString"));

	// create subscription
	SubscriptionPtr s(new Subscription());
	AbstractFilterPtr f(new ScopeFilter("xcf://blah"));
	s->appendFilter(f);

	// register event handler
	Action a = boost::bind(&handler, _1);
	s->appendAction(a);

	// match event and dispatch
	if (s->match(e)) (*s->getActions())(e);
}
