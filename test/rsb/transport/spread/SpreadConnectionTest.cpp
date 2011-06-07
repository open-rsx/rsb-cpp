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

    SpreadConnectionPtr sendConnection(new SpreadConnection("send"));
    sendConnection->activate();

    SpreadConnectionPtr receiveConnection(new SpreadConnection("receive"));
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
    } catch (CommException &e) {
        // this is desired
    }

}
