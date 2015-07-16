/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
 *
 * ============================================================ */

#include <string>

#include <rsc/misc/UUID.h>
#include <rsc/threading/SynchronizedQueue.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/thread.hpp>

#include <rsc/misc/langutils.h>

#include "rsb/eventprocessing/ParallelEventReceivingStrategy.h"
#include "rsb/util/QueuePushHandler.h"
#include "rsb/filter/ScopeFilter.h"
#include "rsb/MetaData.h"
#include "rsb/EventId.h"
#include "rsb/Scope.h"

using namespace std;
using namespace rsb;
using namespace rsb::eventprocessing;
using namespace testing;
using namespace rsc::threading;
using namespace rsb::filter;
using namespace rsb::util;

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

    boost::uint64_t stopWaitingHere = rsc::misc::currentTimeMillis() + 10000;
    boost::shared_ptr<string> queueResult;
    while (rsc::misc::currentTimeMillis() < stopWaitingHere) {
        try {
            queueResult = okQueue->tryPop();
            break;
        } catch (rsc::threading::QueueEmptyException& e){
            boost::this_thread::sleep(boost::posix_time::millisec(20));
            continue;
        }
    }
    EXPECT_TRUE(queueResult);
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

    boost::uint64_t stopWaitingHere = rsc::misc::currentTimeMillis() + 10000;
    boost::shared_ptr<string> queueResult;
    while (rsc::misc::currentTimeMillis() < stopWaitingHere) {
        try {
            queueResult = okQueue->tryPop();
            break;
        } catch (rsc::threading::QueueEmptyException& e){
            boost::this_thread::sleep(boost::posix_time::millisec(20));
            continue;
        }
    }
    EXPECT_TRUE(queueResult);
    EXPECT_TRUE(okQueue->empty());

}

class ErrorGeneratingHandler: public rsb::Handler {
public:

    ErrorGeneratingHandler(const bool& catchAll) :
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

    processor.setHandlerErrorStrategy(ParticipantConfig::ERROR_STRATEGY_LOG);
    processor.handle(event);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    processor.setHandlerErrorStrategy(ParticipantConfig::ERROR_STRATEGY_PRINT);
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

    processor.setHandlerErrorStrategy(ParticipantConfig::ERROR_STRATEGY_LOG);
    processor.handle(event);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    processor.setHandlerErrorStrategy(ParticipantConfig::ERROR_STRATEGY_PRINT);
    processor.handle(event);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

}
