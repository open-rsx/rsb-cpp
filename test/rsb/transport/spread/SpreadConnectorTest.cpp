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

TEST(SpreadConnectorTest, testHierarchySending)
{

    const unsigned int numEvents = 10;
    const Scope sendScope("/this/is/a/long/test");

    QualityOfServiceSpec qosSpecs(QualityOfServiceSpec::ORDERED,
            QualityOfServiceSpec::RELIABLE);

    OutConnectorPtr out(new rsb::spread::OutConnector());
    ASSERT_NO_THROW(out->activate());
    out->setQualityOfServiceSpecs(qosSpecs);

    vector<boost::shared_ptr<WaitingObserver> > observers;
    vector<InConnectorPtr> inConnectors;

    vector<Scope> receiveScopes = sendScope.superScopes(true);
    for (vector<Scope>::const_iterator receiveScopeIt = receiveScopes.begin(); receiveScopeIt != receiveScopes.end(); ++receiveScopeIt) {

        Scope receiveScope = *receiveScopeIt;

        // in connector
        InConnectorPtr in(new rsb::spread::InConnector());
        in->activate();
        in->setQualityOfServiceSpecs(qosSpecs);

        // filter for joining test group
        FilterPtr f = FilterPtr(new ScopeFilter(receiveScope));
        f->notifyObserver(in, FilterAction::ADD);

        unsigned int expectedEvents = numEvents;
        if (receiveScope == Scope("/")) {
            expectedEvents = 2 * numEvents;
        }

        boost::shared_ptr<WaitingObserver> observer(new WaitingObserver(expectedEvents, receiveScope));
        in->setObserver(HandlerPtr(new EventFunctionHandler(boost::bind(&WaitingObserver::handler, observer, _1))));
        observers.push_back(observer);

        inConnectors.push_back(in);

    }

    // why do I have to sleep here? Otherwise sometimes the first handler does
    // not get all events on scope "/"
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));

    // send events
    boost::shared_ptr<InformerTask> source(new InformerTask(out, sendScope, numEvents, 10000));
    TaskExecutorPtr exec(new ThreadedTaskExecutor);
    exec->schedule(source);
    source->waitDone();

    for (vector<boost::shared_ptr<WaitingObserver> >::iterator observerIt = observers.begin();
            observerIt != observers.end(); ++observerIt) {

        boost::shared_ptr<WaitingObserver> observer = *observerIt;
        bool ok = observer->waitReceived(40000);
        ASSERT_TRUE(ok) << "Observer on scope " << observer->getScope() << " did not receive events.";

        // the root observer will get all events, also the garbage created by
        // the informer task...
        if (observer->getScope() == Scope("/")) {
            ASSERT_EQ(source->getEvents().size() * 2, observer->getEvents().size());
            continue;
        }

        // compare sent and received events
        // ordering must be always correct, because we use appropriate QoS settings
        ASSERT_EQ(source->getEvents().size(), observer->getEvents().size()) << "Observer on " << observer->getScope() << " did not receive enough events.";
        for (unsigned int i = 0; i < source->getEvents().size(); ++i) {
            EventPtr sent = source->getEvents()[i];
            EventPtr received = observer->getEvents()[i];
            EXPECT_EQ(sent->getId(), received->getId());
            EXPECT_EQ(sent->getType(), received->getType());
            EXPECT_EQ(sent->getScope(), received->getScope());
        }

    }

}

/**
 * Use a value-parametrized test for different data sizes to test data
 * splitting.
 *
 * @author jwienke
 */
class SpreadConnectorRoundtripTest: public ::testing::TestWithParam<unsigned int> {
};

TEST_P(SpreadConnectorRoundtripTest, roundtrip)
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
    const Scope scope("/blah");
    FilterPtr f = FilterPtr(new ScopeFilter(scope));
    f->notifyObserver(in, FilterAction::ADD);

    // domain objects
    const unsigned int numEvents = 100;
    boost::shared_ptr<InformerTask> source(new InformerTask(out, scope, numEvents, GetParam()));
    WaitingObserver observer(numEvents, scope);
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

}

INSTANTIATE_TEST_CASE_P(RoundtripTest,
        SpreadConnectorRoundtripTest,
        ::testing::Values(1000, 100000, 350000))
;
