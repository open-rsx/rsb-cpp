/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "ConnectorTest.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsc/threading/ThreadedTaskExecutor.h>
#include <rsc/misc/langutils.h>

#include "rsb/Handler.h"
#include "rsb/MetaData.h"
#include "rsb/EventId.h"
#include "rsb/Scope.h"

#include "rsb/filter/Filter.h"

#include "rsb/transport/Connector.h"

#include "../InformerTask.h"

#include "testhelpers.h"
#include "testconfig.h"

#include <rsb/Factory.h>

using namespace std;

using namespace testing;

using namespace rsc::threading;

using namespace rsb;
using namespace rsb::filter;
using namespace rsb::transport;

using namespace rsb::test;

int pullInConnectorTest() {
    return 0;
}

TEST_P(ConnectorTest, testConstruction) {
    rsb::getFactory();
    {
        InPullConnectorPtr connector;
        ASSERT_NO_THROW(connector = GetParam().createInPullConnector());
    }
    {
        InPushConnectorPtr connector;
        ASSERT_NO_THROW(connector = GetParam().createInPushConnector());
    }
    {
        OutConnectorPtr connector;
        ASSERT_NO_THROW(connector = GetParam().createOutConnector());
    }
}

TEST_P(ConnectorTest, testConnection) {
    rsb::getFactory();

    {
        InPullConnectorPtr connector;
        ASSERT_NO_THROW(connector = GetParam().createInPullConnector());
        ASSERT_NO_THROW(connector->activate());
        ASSERT_NO_THROW(connector->deactivate());
    }

    {
        InPushConnectorPtr connector;
        ASSERT_NO_THROW(connector = GetParam().createInPushConnector());
        ASSERT_NO_THROW(connector->activate());
        ASSERT_NO_THROW(connector->deactivate());
    }

    {
        OutConnectorPtr connector;
        ASSERT_NO_THROW(connector = GetParam().createOutConnector());
        ASSERT_NO_THROW(connector->activate());
        ASSERT_NO_THROW(connector->deactivate());
    }
}

TEST_P(ConnectorTest, testSendLongGroupNames) {

    OutConnectorPtr out = GetParam().createOutConnector();
    out->activate();

    Scope longScope(
            "/this/is/a/very/long/scope/which/will/not/fit/into/thirty/two/chars");

    EventPtr e(new Event());
    e->setScope(longScope);
    e->setType(rsc::runtime::typeName<string>());
    e->setData(boost::shared_ptr<string>(new string("fooo")));
    e->setId(rsc::misc::UUID(), 2323);
    out->handle(e);

    out->deactivate();
}

TEST_P(ConnectorTest, testSetSendTime) {

    OutConnectorPtr out = GetParam().createOutConnector();
    out->activate();

    EventPtr e(new Event());
    e->setScope(Scope("/random/useless/scope"));
    e->setType(rsc::runtime::typeName<string>());
    e->setData(boost::shared_ptr<string>(new string("fooo")));
    e->setId(rsc::misc::UUID(), 2323);

    boost::uint64_t beforeSend = rsc::misc::currentTimeMicros();
    out->handle(e);
    boost::uint64_t afterSend = rsc::misc::currentTimeMicros();

    EXPECT_NE(boost::uint64_t(0), e->getMetaData().getSendTime());
    EXPECT_GE(e->getMetaData().getSendTime(), beforeSend);
    EXPECT_LE(e->getMetaData().getSendTime(), afterSend);

    out->deactivate();

}

TEST_P(ConnectorTest, testHierarchySending) {

    const unsigned int numEvents = 10;
    const Scope sendScope("/this/is/a/long/test");

    QualityOfServiceSpec qosSpecs(QualityOfServiceSpec::ORDERED,
            QualityOfServiceSpec::RELIABLE);

    OutConnectorPtr out = GetParam().createOutConnector();
    out->setQualityOfServiceSpecs(qosSpecs);
    out->activate();
    vector<boost::shared_ptr<WaitingObserver> > observers;
    vector<InConnectorPtr> inConnectors;

    vector<Scope> receiveScopes = sendScope.superScopes(true);
    for (vector<Scope>::const_iterator receiveScopeIt = receiveScopes.begin();
            receiveScopeIt != receiveScopes.end(); ++receiveScopeIt) {

        Scope receiveScope = *receiveScopeIt;

        // in connector
        InPushConnectorPtr in = GetParam().createInPushConnector();
        in->setQualityOfServiceSpecs(qosSpecs);
        in->setScope(receiveScope);
        in->activate();

        unsigned int expectedEvents = numEvents;
        if (receiveScope == Scope("/")) {
            expectedEvents = 2 * numEvents;
        }

        boost::shared_ptr<WaitingObserver> observer(
                new WaitingObserver(expectedEvents, receiveScope));
        in->addHandler(
                HandlerPtr(
                        new EventFunctionHandler(
                                boost::bind(&WaitingObserver::handler, observer,
                                        _1))));
        observers.push_back(observer);

        inConnectors.push_back(in);

    }

    // why do I have to sleep here? Otherwise sometimes the first handler does
    // not get all events on scope "/"
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));

    // send events
    boost::shared_ptr<InformerTask> source(
            new InformerTask(out, sendScope, numEvents, 10000));
    TaskExecutorPtr exec(new ThreadedTaskExecutor);
    exec->schedule(source);
    source->waitDone();

    for (vector<boost::shared_ptr<WaitingObserver> >::iterator observerIt =
            observers.begin(); observerIt != observers.end(); ++observerIt) {

        boost::shared_ptr<WaitingObserver> observer = *observerIt;
        bool ok = observer->waitReceived(40000);
        ASSERT_TRUE(ok) << "Observer on scope " << observer->getScope()
                        << " did not receive events.";

        // the root observer will get all events, also the garbage created by
        // the informer task...
        if (observer->getScope() == Scope("/")) {
            ASSERT_EQ(source->getEvents().size() * 2,
                    observer->getEvents().size());
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
            EXPECT_EQ(*sent->getScopePtr(), *received->getScopePtr());
        }

    }

}

TEST_P(ConnectorTest, testRoundtripDynamicScopes) {
    // this test will create random scopes in order to ensure that flushing any caches works correctly

    // task execution service
    TaskExecutorPtr exec(new ThreadedTaskExecutor);

    QualityOfServiceSpec qosSpecs(QualityOfServiceSpec::ORDERED,
            QualityOfServiceSpec::RELIABLE);

    OutConnectorPtr out = GetParam().createOutConnector();
    out->setQualityOfServiceSpecs(qosSpecs);
    ASSERT_NO_THROW(out->activate());

    // in connector
    InPushConnectorPtr in = GetParam().createInPushConnector();
    in->setQualityOfServiceSpecs(qosSpecs);
    in->setScope(Scope("/"));
    ASSERT_NO_THROW(in->activate());

    const unsigned int repetitions = 400;
    WaitingObserver observer(repetitions * 2, Scope("/"));
    in->addHandler(
            HandlerPtr(
                    new EventFunctionHandler(
                            boost::bind(&WaitingObserver::handler,
                                    &observer, _1))));

    for (unsigned int repetition = 0; repetition < repetitions; ++repetition) {

        const Scope scope("/" + rsc::misc::randAlnumStr(15));

        boost::shared_ptr<InformerTask> source(
                new InformerTask(out, scope, 1, 50));
        exec->schedule(source);
        source->waitDone();

    }

    // compare sent and received events
    observer.waitReceived();
    EXPECT_EQ(repetitions * 2, observer.getEvents().size()) << "Roundtrip produced a different number of received messages";

    out->deactivate();
    in->deactivate();

}

TEST_P(ConnectorTest, testRoundtrip) {

    // task execution service
    TaskExecutorPtr exec(new ThreadedTaskExecutor);

    QualityOfServiceSpec qosSpecs(QualityOfServiceSpec::ORDERED,
            QualityOfServiceSpec::RELIABLE);

    // joining test group
    const Scope scope("/blah");

    vector<unsigned int> sizes;
    sizes.push_back(1000);
    sizes.push_back(100000);
    sizes.push_back(350000);
    for (vector<unsigned int>::const_iterator sizeIt = sizes.begin();
            sizeIt != sizes.end(); ++sizeIt) {

        // in connector
        InPushConnectorPtr in = GetParam().createInPushConnector();
        in->setQualityOfServiceSpecs(qosSpecs);
        in->setScope(scope);
        ASSERT_NO_THROW(in->activate());
        OutConnectorPtr out = GetParam().createOutConnector();
        out->setQualityOfServiceSpecs(qosSpecs);
        ASSERT_NO_THROW(out->activate());

        boost::uint64_t sendTime = rsc::misc::currentTimeMicros();

        // domain objects
        const unsigned int numEvents = 100;
        boost::shared_ptr<InformerTask> source(
                new InformerTask(out, scope, numEvents, *sizeIt));
        WaitingObserver observer(numEvents, scope);
        in->addHandler(
                HandlerPtr(
                        new EventFunctionHandler(
                                boost::bind(&WaitingObserver::handler,
                                        &observer, _1))));

        // activate port and schedule informer
        exec->schedule(source);

        observer.waitReceived();
        source->waitDone();

        // compare sent and received events
        // ordering must be always correct, because we use appropriate QoS settings
        ASSERT_EQ(source->getEvents().size(), observer.getEvents().size()) << "Roundtrip produced a different number of received messages than sent for message size " << *sizeIt;
        for (unsigned int i = 0; i < source->getEvents().size(); ++i) {
            EventPtr sent = source->getEvents()[i];
            EventPtr received = observer.getEvents()[i];
            EXPECT_EQ(sent->getId(), received->getId()) << "Error matching event id for index " << i << " and message size " << *sizeIt;
            EXPECT_EQ(sent->getType(), received->getType()) << "Error matching event type for index " << i << " and message size " << *sizeIt;
            EXPECT_EQ(*sent->getScopePtr(), *received->getScopePtr()) << "Error matching event scope for index " << i << " and message size " << *sizeIt;

            // causes
            EXPECT_EQ(sent->getCauses(), received->getCauses());

            // meta data
            EXPECT_EQ(sent->getId().getParticipantId(),
                    received->getId().getParticipantId());

            EXPECT_GE(received->getMetaData().getCreateTime(), sendTime);
            EXPECT_LE(received->getMetaData().getCreateTime(),
                    rsc::misc::currentTimeMicros());

            EXPECT_GE(received->getMetaData().getSendTime(), sendTime);
            EXPECT_LE(received->getMetaData().getSendTime(),
                    rsc::misc::currentTimeMicros());
            EXPECT_GE(received->getMetaData().getSendTime(),
                    received->getMetaData().getCreateTime());

            EXPECT_GE(received->getMetaData().getReceiveTime(), sendTime);
            EXPECT_LE(received->getMetaData().getReceiveTime(),
                    rsc::misc::currentTimeMicros());
            EXPECT_GE(received->getMetaData().getReceiveTime(),
                    received->getMetaData().getSendTime());
        }

    }

}
