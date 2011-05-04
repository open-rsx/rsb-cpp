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
#include <rsc/threading/RepetitiveTask.h>

#include "rsb/eventprocessing/InRouteConfigurator.h"
#include "rsb/eventprocessing/OutRouteConfigurator.h"
#include "rsb/transport/Factory.h"
#include "rsb/transport/Connector.h"
#include "rsb/transport/transports.h"
#include "InformerTask.h"
#include "rsb/Handler.h"
#include "rsb/filter/Filter.h"
#include "rsb/filter/ScopeFilter.h"
#include "rsb/converter/converters.h"
#include "rsb/Factory.h"
#include "rsb/Informer.h"

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

class UserInformerTask: public rsc::threading::RepetitiveTask {
public:

    UserInformerTask(Informer<string>::Ptr informer,
            const unsigned int &numEvents) :
        informer(informer), numEvents(numEvents), sentEvents(0) {

    }

    virtual ~UserInformerTask() {
    }

    void execute() {
        informer->publish(boost::shared_ptr<string>(new string("hello world")));
        ++sentEvents;
        if (sentEvents == numEvents) {
            cancel();
        }
    }

private:
    Informer<string>::Ptr informer;
    unsigned int numEvents;
    unsigned int sentEvents;

};

TEST(RoundtripTest, testRoundtrip)
{

    Factory::killInstance();
    Factory &factory = Factory::getInstance();
    ParticipantConfig config = factory.getDefaultParticipantConfig();
    ParticipantConfig::Transport spreadTransport =
            config.getTransport("spread");
    rsc::runtime::Properties p = spreadTransport.getOptions();
    p.set<unsigned int> ("port", SPREAD_PORT);
    spreadTransport.setOptions(p);
    config.addTransport(spreadTransport);
    factory.setDefaultParticipantConfig(config);

    const Scope scope("/blah");

    ListenerPtr listener = factory.createListener(scope);
    Informer<string>::Ptr informer = factory.createInformer<string> (scope);

    // domain objects
    unsigned int numEvents = 10;
    boost::shared_ptr<UserInformerTask> source(
            new UserInformerTask(informer, 10));
    WaitingObserver observer(numEvents, scope);

    listener->addHandler(
            rsb::HandlerPtr(
                    new EventFunctionHandler(
                            boost::bind(&WaitingObserver::handler, &observer,
                                    _1))));

    // task execution service
    TaskExecutorPtr exec(new ThreadedTaskExecutor);
    exec->schedule(source);

    source->waitDone();
    observer.waitReceived();

}

int main(int argc, char* argv[]) {

    setupLogging();

    SubprocessPtr spread = startSpread();
    boost::this_thread::sleep(boost::posix_time::seconds(2));

    InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();

}
