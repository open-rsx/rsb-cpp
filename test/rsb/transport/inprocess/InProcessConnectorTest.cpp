/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsc/threading/ThreadedTaskExecutor.h>

#include "rsb/transport/inprocess/InConnector.h"
#include "rsb/transport/inprocess/OutConnector.h"

#include "../../InformerTask.h"

#include "testhelpers.h"

using namespace std;

using namespace testing;

using namespace rsc::threading;

using namespace rsb;
using namespace rsb::inprocess;

using namespace rsb::test;

TEST(InprocessConnectorTest, testConstruction)
{
Scope();
    ASSERT_NO_THROW(transport::InConnectorPtr in(new InConnector()));
      ASSERT_NO_THROW(transport::OutConnectorPtr out(new OutConnector()));
}

TEST(InprocessConnectorTest, testActivate)
{
    transport::InConnectorPtr in(new InConnector());
    ASSERT_NO_THROW(in->activate());
    transport::OutConnectorPtr out(new OutConnector());
    ASSERT_NO_THROW(out->activate());
}

TEST(InprocessConnectorTest, testRoundtrip)
{
InConnectorPtr in(new InConnector());
    in->setScope(Scope("/blah"));
    in->activate();
    transport::OutConnectorPtr out(new OutConnector());
    out->activate();

    unsigned int numEvents = 3;
    boost::shared_ptr<InformerTask> source(new InformerTask(out, Scope("/blah"), 1, 100));
    WaitingObserver observer(numEvents);
    in->setObserver(HandlerPtr(new EventFunctionHandler(boost::bind(&WaitingObserver::handler, &observer, _1))));

    // Send some events
    TaskExecutorPtr exec(new ThreadedTaskExecutor);
    exec->schedule(source);
    source->cancel();
    source->waitDone();

    out->push(EventPtr(new Event()));

    {
        EventPtr e(new Event());
        e->setScope(Scope("/blah"));
        out->push(e);
    }

    {
        EventPtr e(new Event());
        e->setScope(Scope("/blah/foo"));
        out->push(e);
    }

    {
        EventPtr e(new Event());
        e->setScope(Scope("/bar"));
        out->push(e);
    }

    observer.waitReceived();

    ASSERT_EQ(numEvents, observer.getEvents().size());
    for (unsigned int i = 0; i < source->getEvents().size(); ++i) {
        EventPtr sent = source->getEvents()[i];
        EventPtr received = observer.getEvents()[i];
        EXPECT_EQ(sent->getId(), received->getId());
        EXPECT_EQ(sent->getType(), received->getType());
        EXPECT_EQ(sent->getScope(), received->getScope());
        }
}
