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

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsc/subprocess/Subprocess.h>
#include <rsc/threading/ThreadedTaskExecutor.h>

#include "rsb/eventprocessing/Router.h"
#include "rsb/transport/Connector.h"
#include "rsb/transport/spread/SpreadConnector.h"
#include "InformerTask.h"
#include "rsb/Subscription.h"
#include "rsb/filter/Filter.h"
#include "rsb/filter/ScopeFilter.h"
#include "rsb/introspection/introspection.h"

#include "testhelpers.h"

using namespace std;
using namespace rsb;
using namespace rsb::test;
using namespace rsb::filter;
using namespace rsb::transport;
using namespace rsb::eventprocessing;
using namespace rsb::spread;
using namespace rsc::subprocess;
using namespace testing;
using namespace rsc::threading;

TEST(RSBTest, testRoundtrip)
{

	introspection::registerIntrospectionConverters();

	// task execution service
	TaskExecutorPtr exec(new ThreadedTaskExecutor);

	// router instantiation
	RouterPtr r(new Router(Factory::SPREAD, Factory::SPREAD));
	r->activate();

	// create subscription
	SubscriptionPtr s(new Subscription());
	FilterPtr f(new ScopeFilter("xcf://blah"));
	s->appendFilter(f);

	// domain objects
	unsigned int numEvents = 10;
	boost::shared_ptr<InformerTask> source(
			new InformerTask(r->getOutConnector(), 10));
	WaitingObserver observer(numEvents);
	s->appendAction(boost::bind(&WaitingObserver::handler, &observer, _1));

	// add subscription to router
	r->subscribe(s);

	// activate port and schedule informer
	exec->schedule(source);

	observer.waitReceived();

	// port is deactivated through dtr
	cerr << "RSBTest finished" << endl;
}

int main(int argc, char* argv[]) {

	setupLogging();

	SubprocessPtr spread = startSpread();
	boost::this_thread::sleep(boost::posix_time::seconds(2));

	InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();

}
