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

#include <iostream>

#include <stdlib.h>
#include <math.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/timer.hpp>

#include <rsc/logging/Logger.h>

#include <rsb/Subscriber.h>
#include <rsb/Subscription.h>
#include <rsb/Handler.h>
#include <rsb/filter/ScopeFilter.h>
#include <rsb/RSBFactory.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;
using namespace rsb::filter;

class MyDataHandler: public DataHandler<string> {
public:
	MyDataHandler() {
	}

	void notify(boost::shared_ptr<string> e) {
		cout << "Data received: " << *e << endl;
	}

};

int main(int argc, char **argv) {

	RSBFactory factory;

	LoggerPtr l = Logger::getLogger("receiver");

	boost::timer t;

	SubscriberPtr s = factory.createSubscriber("blub");
	SubscriptionPtr sub(new Subscription());
	if (argc > 1) {
		sub->appendFilter(AbstractFilterPtr(new ScopeFilter(argv[1])));
	} else {
		sub->appendFilter(AbstractFilterPtr(new ScopeFilter("rsb://example/informer")));
	}

	boost::shared_ptr<MyDataHandler> dh(new MyDataHandler());

	// register event handler
	sub->appendHandler(dh);

	s->addSubscription(sub);

	cout << "Subscriber setup finished. Waiting for messages..." << endl;

	while (true) {
		boost::this_thread::sleep(boost::posix_time::seconds(1000));
	}

	return (EXIT_SUCCESS);
}
