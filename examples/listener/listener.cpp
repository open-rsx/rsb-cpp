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

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;
using namespace rsb::filter;

class MyDataHandler: public DataHandler<string> {
public:
	MyDataHandler() :
		count(0) {
	}

	void notify(boost::shared_ptr<string> e) {
		cout << "Received: " << *e << endl;
	}

	long count;
	boost::recursive_mutex m;
	boost::condition cond;
};

int main( int argc, const char* argv[] ) {

	LoggerPtr l = Logger::getLogger("receiver");

	boost::timer t;

	SubscriberPtr s(new Subscriber("blub"));
	SubscriptionPtr sub(new Subscription());

	string uri = "rsb://example/informer";
	for (int i = 0; i < argc; i++) {
			if (argv[i][0]=='r') {
				// assume RSB URL is given
				uri = argv[1];
			}
	}

	cout << "Listening with Data Handler to URL: " << uri << endl;

	AbstractFilterPtr f(new ScopeFilter(uri));
	sub->appendFilter(f);

	boost::shared_ptr<MyDataHandler> dh(new MyDataHandler());

	// register event handler
	sub->appendHandler(dh);

	s->addSubscription(sub);

	cout << "Subscriber setup finished. Waiting for messages..." << endl;

	// wait *here* for shutdown as this is not known to the Subscriber
	{
		boost::recursive_mutex::scoped_lock lock(dh->m);
		while (true) {
			dh->cond.wait(lock);
		}
	}

	return (EXIT_SUCCESS);
}
