/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <string>

#include <rsc/threading/SynchronizedQueue.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/thread.hpp>

#include "rsb/EventProcessor.h"
#include "rsb/Subscription.h"
#include "rsb/QueuePushHandler.h"
#include "rsb/filter/ScopeFilter.h"

using namespace std;
using namespace rsb;
using namespace rsb::internal;
using namespace testing;
using namespace rsc::threading;
using namespace rsb::filter;

TEST(EventProcessorTest, testProcessing)
{

	EventProcessor processor(1);

	boost::shared_ptr<SynchronizedQueue<boost::shared_ptr<string> > > okQueue(
			new SynchronizedQueue<boost::shared_ptr<string> > );
	HandlerPtr okHandler(new QueuePushHandler<string> (okQueue));
	SubscriptionPtr okSubscription(new Subscription);
	const string okScope = "OK";
	okSubscription->appendFilter(AbstractFilterPtr(new ScopeFilter(okScope)));
	okSubscription->appendHandler(okHandler);
	processor.subscribe(okSubscription);

	boost::shared_ptr<SynchronizedQueue<boost::shared_ptr<string> > >
			wrongQueue(new SynchronizedQueue<boost::shared_ptr<string> > );
	HandlerPtr wrongHandler(new QueuePushHandler<string> (wrongQueue));
	SubscriptionPtr wrongSubscription(new Subscription);
	const string wrongScope = "WRONG";
	wrongSubscription->appendFilter(AbstractFilterPtr(new ScopeFilter(
			wrongScope)));
	wrongSubscription->appendHandler(wrongHandler);
	processor.subscribe(wrongSubscription);

	RSBEventPtr event(new RSBEvent);
	event->setURI(okScope);
	event->setData(boost::shared_ptr<string>(new string("hello")));

	processor.process(event);

	boost::this_thread::sleep(boost::posix_time::millisec(500));

	EXPECT_FALSE(okQueue->empty());
	EXPECT_TRUE(wrongQueue->empty());

}
