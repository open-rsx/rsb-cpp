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

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsc/misc/langutils.h>
#include <rsc/misc/UUID.h>

#include "rsb/transport/spread/SpreadMessage.h"

using namespace std;
using namespace rsb;
using namespace rsb::spread;
using namespace testing;

TEST(SpreadMessageTest, testConstruction)
{

    {
        SpreadMessage m;
        EXPECT_EQ(SpreadMessage::OTHER, m.getType());
        EXPECT_EQ(SpreadMessage::UNRELIABLE, m.getQOS());
        EXPECT_EQ(string(""), string(m.getData()));
        EXPECT_EQ(size_t(0), m.getGroupCount());
    }

    {
        SpreadMessage m(SpreadMessage::MEMBERSHIP);
        EXPECT_EQ(SpreadMessage::MEMBERSHIP, m.getType());
        EXPECT_EQ(SpreadMessage::UNRELIABLE, m.getQOS());
        EXPECT_EQ(string(""), string(m.getData()));
        EXPECT_EQ(size_t(0), m.getGroupCount());
    }

    {
        SpreadMessage m(string("data"));
        EXPECT_EQ(SpreadMessage::OTHER, m.getType());
        EXPECT_EQ(SpreadMessage::UNRELIABLE, m.getQOS());
        EXPECT_EQ(string("data"), string(m.getData()));
        EXPECT_EQ(size_t(0), m.getGroupCount());
    }

    {
        const char* data = "foo";
        SpreadMessage m(data);
        EXPECT_EQ(SpreadMessage::OTHER, m.getType());
        EXPECT_EQ(SpreadMessage::UNRELIABLE, m.getQOS());
        EXPECT_EQ(string(data), string(m.getData()));
        EXPECT_EQ(size_t(0), m.getGroupCount());
    }

}

TEST(SpreadMessageTest, testReset)
{

    SpreadMessage m;
    m.setType(SpreadMessage::MEMBERSHIP);
    m.setQOS(SpreadMessage::UNRELIABLE);
    m.setData("foooo");
    m.addGroup("blar");
    m.addGroup("blaradsasd");

    m.reset();

    EXPECT_EQ(SpreadMessage::OTHER, m.getType());
    EXPECT_EQ(SpreadMessage::UNRELIABLE, m.getQOS());
    EXPECT_EQ(string(""), string(m.getData()));
    EXPECT_EQ(size_t(0), m.getGroupCount());

}
