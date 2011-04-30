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

#include <rsc/threading/ThreadedTaskExecutor.h>

#include "rsb/transport/Connector.h"
#include "rsb/transport/spread/SpreadConnector.h"
#include "rsb/transport/spread/InConnector.h"
#include "rsb/transport/spread/OutConnector.h"
#include "rsb/filter/Filter.h"

#include "../../InformerTask.h"

#include "testhelpers.h"

using namespace std;
using namespace rsb;
using namespace rsb::test;
using namespace rsb::filter;
using namespace rsb::transport;
using namespace rsb::spread;
using namespace testing;
using namespace rsc::threading;

TEST(SpreadConnectorTest, testConstruction)
{
    ASSERT_NO_THROW(SpreadConnectorPtr p(new rsb::spread::SpreadConnector()));
    ASSERT_NO_THROW(InConnectorPtr in(new rsb::spread::InConnector()));
    ASSERT_NO_THROW(OutConnectorPtr out(new rsb::spread::OutConnector()));
}

TEST(SpreadConnectorTest, testConnnection)
{
    SpreadConnectorPtr p(new rsb::spread::SpreadConnector());
    ASSERT_NO_THROW(p->activate());
}

TEST(SpreadConnectorTest, testRoundtrip)
{

    // task execution service
    TaskExecutorPtr exec(new ThreadedTaskExecutor);

    QualityOfServiceSpec qosSpecs(QualityOfServiceSpec::ORDERED,
            QualityOfServiceSpec::RELIABLE);
    // in connector
    InConnectorPtr in(new rsb::spread::InConnector());
    ASSERT_NO_THROW(in->activate());
    in->setQualityOfServiceSpecs(qosSpecs);

    OutConnectorPtr out(new rsb::spread::OutConnector());
    ASSERT_NO_THROW(out->activate());
    out->setQualityOfServiceSpecs(qosSpecs);

    // filter for joining test group
    FilterPtr f = FilterPtr(new ScopeFilter(Scope("/blah")));
    f->notifyObserver(in, FilterAction::ADD);

    // domain objects
    const unsigned int numEvents = 100;
    boost::shared_ptr<InformerTask> source(new InformerTask(out, numEvents));
    WaitingObserver observer(numEvents);
    in->setObserver(HandlerPtr(new EventFunctionHandler(boost::bind(&WaitingObserver::handler, &observer, _1))));

    // activate port and schedule informer
    exec->schedule(source);

    observer.waitReceived();
    source->waitDone();

    // compare sent and received events
    // ordering must be always correct, because we use appropriate QoS settings
    ASSERT_EQ(source->getEvents().size(), observer.getEvents().size());
    for (unsigned int i = 0; i < source->getEvents().size(); ++i) {
        EventPtr sent = source->getEvents()[i];
        EventPtr received = observer.getEvents()[i];
        EXPECT_EQ(sent->getId(), received->getId());
        EXPECT_EQ(sent->getType(), received->getType());
        EXPECT_EQ(sent->getScope(), received->getScope());
    }

    // port is deactivated through dtr
    cerr << "SpreadProcessTest finished" << endl;

}
