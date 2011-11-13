/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <rsc/misc/IllegalStateException.h>
#include <rsc/misc/langutils.h>

#include "rsb/transport/spread/SpreadConnection.h"
#include "rsb/transport/spread/MembershipManager.h"
#include "rsb/CommException.h"

#include "testconfig.h"

using namespace std;
using namespace rsb;
using namespace rsb::spread;
using namespace testing;

TEST(SpreadConnectionTest, testErrorOnBufferSaturation)
{

    const unsigned int bufferSize = 8000;

    SpreadConnectionPtr sendConnection(new SpreadConnection("send", "localhost", SPREAD_PORT));
    sendConnection->activate();

    SpreadConnectionPtr receiveConnection(new SpreadConnection("receive", "localhost", SPREAD_PORT));
    receiveConnection->activate();

    const string groupName = "dasdasd";
    MembershipManager manager;
    manager.join(groupName, receiveConnection);

    // fill the buffer
    SpreadMessage message(SpreadMessage::REGULAR);
    message.setData(rsc::misc::randAlnumStr(100000));
    message.addGroup(groupName);
    for (unsigned int i = 0; i < bufferSize; ++i) {
        sendConnection->send(message);
    }

    // read from the buffer that is flooded
    try {
        SpreadMessagePtr receiveMessage(new SpreadMessage);
        for (unsigned int i = 0; i < bufferSize; ++i) {
            receiveConnection->receive(receiveMessage);
        }
        FAIL() << "Connection did not throw an error on buffer saturation." ;
    } catch (CommException& e) {
        // this is desired
    }

}

TEST(SpreadConnectionTest, testActivationStateChecks)
{

    SpreadConnection con("activity", "localhost", SPREAD_PORT);

    SpreadMessage sendMsg("foo");
    sendMsg.addGroup("blubb");
    EXPECT_THROW(con.send(sendMsg), rsc::misc::IllegalStateException);

    SpreadMessagePtr receiveMsg(new SpreadMessage);
    EXPECT_THROW(con.receive(receiveMsg), rsc::misc::IllegalStateException);

    EXPECT_THROW(con.interruptReceive(), rsc::misc::IllegalStateException);

    EXPECT_THROW(con.getMailbox(), rsc::misc::IllegalStateException);

    EXPECT_THROW(con.deactivate(), rsc::misc::IllegalStateException) << "duplicated deactivation must not be possible";

    con.activate();
    EXPECT_THROW(con.activate(), rsc::misc::IllegalStateException) << "duplicated activation must not be possible";

}

TEST(SpreadConnectionTest, testDeactivateDestructorNotRequired)
{
    SpreadConnection con("nothrow", "localhost", SPREAD_PORT);
}

void receiveTester(SpreadConnectionPtr con) {

    try {
        SpreadMessagePtr msg(new SpreadMessage);
        while (true) {
            con->receive(msg);
        }
    } catch (boost::thread_interrupted& e) {
        // this is ok
    } catch (rsc::misc::IllegalStateException& e) {
        // this also, because the connectionmay have been disable before
    }

}

TEST(SpreadConnectionTest, testInterruptReceive)
{

    SpreadConnectionPtr con(new SpreadConnection("interrupt", "localhost", SPREAD_PORT));
    con->activate();

    // should be ok
    con->interruptReceive();

    boost::thread receiveThread(boost::bind(&receiveTester, con));
    // let the thread start to receive
    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    con->interruptReceive();
    // the thread must be now join in the next time
    receiveThread.join();

    con->deactivate();

}
