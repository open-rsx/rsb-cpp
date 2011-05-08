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

#include "rsb/eventprocessing/ParallelEventReceivingStrategy.h"
#include "rsb/QueuePushHandler.h"
#include "rsb/filter/ScopeFilter.h"

using namespace std;
using namespace rsb;
using namespace rsb::eventprocessing;
using namespace testing;
using namespace rsc::threading;
using namespace rsb::filter;

TEST(ParallelEventReceivingStrategyTest, testReceiving)
{

    ParallelEventReceivingStrategy processor(1);
    const Scope okScope("/OK");
    processor.addFilter(FilterPtr(new ScopeFilter(okScope)));

    boost::shared_ptr<SynchronizedQueue<boost::shared_ptr<string> > > okQueue(
            new SynchronizedQueue<boost::shared_ptr<string> > );
    rsb::HandlerPtr okHandler(new QueuePushHandler<string> (okQueue));
    processor.addHandler(okHandler, true);

    {
        EventPtr event(new Event);
        event->setScope(okScope);
        event->setData(boost::shared_ptr<string>(new string("hello")));

        processor.handle(event);
    }
    {
        EventPtr event(new Event);
        event->setScope(Scope("/This/Is/wrong"));
        event->setData(boost::shared_ptr<string>(new string("hello")));

        processor.handle(event);
    }

    boost::this_thread::sleep(boost::posix_time::millisec(500));

    EXPECT_FALSE(okQueue->empty());
    okQueue->pop();
    EXPECT_TRUE(okQueue->empty());

}

TEST(ParallelEventReceivingStrategyTest, testMethodFiltering)
{

    ParallelEventReceivingStrategy processor(1);
    const Scope okScope("/OK");
    const string desiredMethod = "FooMethod";
    processor.addFilter(FilterPtr(new ScopeFilter(okScope)));

    boost::shared_ptr<SynchronizedQueue<boost::shared_ptr<string> > > okQueue(
            new SynchronizedQueue<boost::shared_ptr<string> > );
    rsb::HandlerPtr okHandler(new QueuePushHandler<string> (okQueue, desiredMethod));
    processor.addHandler(okHandler, true);

    {
        // good method
        EventPtr event(new Event);
        event->setScope(okScope);
        event->setMethod(desiredMethod);
        event->setData(boost::shared_ptr<string>(new string("hello")));

        processor.handle(event);
    }
    {
        // no method at all
        EventPtr event(new Event);
        event->setScope(okScope);
        event->setData(boost::shared_ptr<string>(new string("hello")));

        processor.handle(event);
    }
    {
        // no method at all
        EventPtr event(new Event);
        event->setScope(okScope);
        event->setMethod("wrong method");
        event->setData(boost::shared_ptr<string>(new string("hello")));

        processor.handle(event);
    }
    boost::this_thread::sleep(boost::posix_time::millisec(500));

    EXPECT_FALSE(okQueue->empty());
    okQueue->pop();
    EXPECT_TRUE(okQueue->empty());

}

class ErrorGeneratingHandler: public rsb::Handler {
public:

    ErrorGeneratingHandler(const bool &catchAll) :
        catchAll(catchAll) {
    }

    virtual ~ErrorGeneratingHandler() {

    }

    string getClassName() const {
        return "ErrorGeneratingHandler";
    }

    void handle(EventPtr /*event*/) {
        if (catchAll) {
            throw "A random string";
        } else {
            throw runtime_error("A random error");
        }
    }

private:
    bool catchAll;

};

TEST(ParallelEventReceivingStrategyTest, testHandlerErrorStrategyException)
{

    ParallelEventReceivingStrategy processor(1);
    rsb::HandlerPtr exceptionHandler(new ErrorGeneratingHandler(false));
    processor.addHandler(exceptionHandler, true);

    // default should log
    EventPtr event(new Event);
    event->setData(boost::shared_ptr<string>(new string("hello")));
    processor.handle(event);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    processor.setHandlerErrorStrategy(ParticipantConfig::LOG);
    processor.handle(event);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    processor.setHandlerErrorStrategy(ParticipantConfig::PRINT);
    processor.handle(event);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

}

TEST(ParallelEventReceivingStrategyTest, testHandlerErrorStrategyCatchAll)
{

    ParallelEventReceivingStrategy processor(1);
    rsb::HandlerPtr exceptionHandler(new ErrorGeneratingHandler(true));
    processor.addHandler(exceptionHandler, true);

    // default should log
    EventPtr event(new Event);
    event->setData(boost::shared_ptr<string>(new string("hello")));
    processor.handle(event);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    processor.setHandlerErrorStrategy(ParticipantConfig::LOG);
    processor.handle(event);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    processor.setHandlerErrorStrategy(ParticipantConfig::PRINT);
    processor.handle(event);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

}
