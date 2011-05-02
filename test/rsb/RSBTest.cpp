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

#include "rsb/eventprocessing/InRouteConfigurator.h"
#include "rsb/eventprocessing/OutRouteConfigurator.h"
#include "rsb/transport/Factory.h"
#include "rsb/transport/Connector.h"
#include "rsb/transport/transports.h"
#include "InformerTask.h"
#include "rsb/Subscription.h"
#include "rsb/filter/Filter.h"
#include "rsb/filter/ScopeFilter.h"
#include "rsb/converter/converters.h"

#include "testhelpers.h"

using namespace std;
using namespace rsb;
using namespace rsb::test;
using namespace rsb::filter;
using namespace rsb::transport;
using namespace rsb::eventprocessing;
using namespace rsc::subprocess;
using namespace testing;
using namespace rsc::threading;

TEST(RSBTest, testRoundtrip)
{

    converter::registerDefaultConverters();
    registerDefaultTransports();

    // task execution service
    TaskExecutorPtr exec(new ThreadedTaskExecutor);

    InConnectorPtr in(InFactory::getInstance().createInst("spread"));
    OutConnectorPtr out(OutFactory::getInstance().createInst("spread"));

    // In- and OutRouteConfigurator instantiation
    InRouteConfiguratorPtr inConfigurator(new InRouteConfigurator(in));
    inConfigurator->activate();
    OutRouteConfiguratorPtr outConfigurator(new OutRouteConfigurator(out));
    outConfigurator->activate();

    // create subscription
    SubscriptionPtr s(new Subscription());
    const Scope scope("/blah");
    FilterPtr f(new ScopeFilter(scope));
    s->appendFilter(f);

    // domain objects
    unsigned int numEvents = 10;
    boost::shared_ptr<InformerTask> source(new InformerTask(out, scope, 10, 1000));
    WaitingObserver observer(numEvents, scope);
    set<HandlerPtr> handlers;
    handlers.insert(
            HandlerPtr(
                    new EventFunctionHandler(
                            boost::bind(&WaitingObserver::handler, &observer,
                                    _1))));

    // add subscription to router
    inConfigurator->subscribe(s, handlers);

    // activate port and schedule informer
    exec->schedule(source);

    observer.waitReceived();

    // port is deactivated through dtr
    cerr << "RSBTest finished" << endl;

    source->cancel();
    source->waitDone();

}

int main(int argc, char* argv[]) {

    setupLogging();

    SubprocessPtr spread = startSpread();
    boost::this_thread::sleep(boost::posix_time::seconds(2));

    InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();

}
