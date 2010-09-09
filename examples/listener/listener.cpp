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

#include <stdlib.h>

#include <iostream>
#include <rsc/logging/Logger.h>
// #include <rsc/logging/propertyconfigurator.h>
#include <math.h>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>

#include <rsb/Subscriber.h>
#include <rsb/Subscription.h>
#include <rsb/Handler.h>
#include <rsb/filter/ScopeFilter.h>
#include <rsc/misc/Timer.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;
using namespace rsb::filter;
using namespace rsb::util;

class MyDataHandler : public DataHandler<string> {
public:
	MyDataHandler() : count(0) {};

	void notify(boost::shared_ptr<string> e) {
		cout << "Data received: " << *e << endl;
		count++;
		if (count == 1200) {
				boost::recursive_mutex::scoped_lock lock(m);
				cond.notify_all();
		}
	}

    long count;
	boost::recursive_mutex m;
	boost::condition cond;
};

int main(void) {

//    std::ostringstream confpath;
//    char *log4cxxPropsEnv = getenv("LOG4CXXPROPS");
//
//    if (log4cxxPropsEnv != NULL) {
//
//        confpath << log4cxxPropsEnv;
//        cout << "Trying log4cxx configuration from file " << confpath.str()
//                << endl;
//
//        try {
//            log4cxx::PropertyConfigurator::configure(confpath.str());
//        } catch (const std::exception& e) {
//            cout << "Trying log4cxx configuration from file " << confpath.str()
//                    << " failed. Using BasicConfigurator." << endl;
//        }
//    }

    LoggerPtr l = Logger::getLogger("receiver");

    TimerPtr t(new Timer("prototype"));

    SubscriberPtr s(new Subscriber("blub"));
    SubscriptionPtr sub(new Subscription());
	AbstractFilterPtr f(new ScopeFilter("rsb://example/informer"));
	sub->appendFilter(f);

	boost::shared_ptr<MyDataHandler> dh(new MyDataHandler());
	boost::shared_ptr<Handler> h = boost::static_pointer_cast<Handler>(dh);

	// register event handler
	sub->appendHandler(h);

	s->addSubscription(sub);

    cout << "Subscriber setup finished. Waiting for messages..." << endl;

	// wait *here* for shutdown as this is not known to the Subscriber
	{
		boost::recursive_mutex::scoped_lock lock(dh->m);
		while (dh->count != 1200) {
			dh->cond.wait(lock);
		}
	}

    t->stop();

    cout << "Last message was sent to the following groups: " << endl;

    cout << "Elapsed time per message (" << dh->count << " messages received): " << t->getElapsed()/dh->count << " ms" << endl;

    return (EXIT_SUCCESS);
}
