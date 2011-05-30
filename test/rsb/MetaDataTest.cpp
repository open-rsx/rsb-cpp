/* ============================================================
 *
 * This file is a part of RSB project
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

#include <stdexcept>

#include <rsc/misc/langutils.h>

#include "rsb/MetaData.h"

using namespace std;
using namespace testing;
using namespace rsb;

TEST(MetaDataTest, testConstruction)
{

    MetaData meta;
    EXPECT_GT(meta.getEventCreationTime(), rsc::misc::currentTimeMicros() - 100000);
    EXPECT_LE(meta.getEventCreationTime(), rsc::misc::currentTimeMicros());
    EXPECT_EQ((boost::uint64_t) 0, meta.getRawReceiveTime());
    EXPECT_EQ((boost::uint64_t) 0, meta.getReceiveTime());
    EXPECT_EQ((boost::uint64_t) 0, meta.getSendTime());
    EXPECT_TRUE(meta.userTimeKeys().empty());

}

TEST(MetaDataTest, testSenderId)
{
    const rsc::misc::UUID id;
    MetaData meta;
    meta.setSenderId(id);
    EXPECT_EQ(id, meta.getSenderId());
}

TEST(MetaDataTest, testDefaultTimesAuto)
{

    MetaData meta;
    meta.setEventCreationTime();
    meta.setSendTime();
    meta.setRawReceiveTime();
    meta.setReceiveTime();

    EXPECT_GT(meta.getEventCreationTime(), rsc::misc::currentTimeMicros() - 100000);
    EXPECT_LE(meta.getEventCreationTime(), rsc::misc::currentTimeMicros());
    EXPECT_GT(meta.getSendTime(), rsc::misc::currentTimeMicros() - 100000);
    EXPECT_LE(meta.getSendTime(), rsc::misc::currentTimeMicros());
    EXPECT_GT(meta.getRawReceiveTime(), rsc::misc::currentTimeMicros() - 100000);
    EXPECT_LE(meta.getRawReceiveTime(), rsc::misc::currentTimeMicros());
    EXPECT_GT(meta.getReceiveTime(), rsc::misc::currentTimeMicros() - 100000);
    EXPECT_LE(meta.getReceiveTime(), rsc::misc::currentTimeMicros());

}

TEST(MetaDataTest, testDefaultTimesManual)
{

    MetaData meta;
    const boost::uint64_t time = 13123;
    meta.setEventCreationTime(time);
    meta.setSendTime(time);
    meta.setRawReceiveTime(time);
    meta.setReceiveTime(time);

    EXPECT_EQ(time, meta.getEventCreationTime());
    EXPECT_EQ(time, meta.getSendTime());
    EXPECT_EQ(time, meta.getRawReceiveTime());
    EXPECT_EQ(time, meta.getReceiveTime());

}

TEST(MetaDataTest, testUserTimes)
{

    MetaData meta;
    const string key = "afdadfasfd";
    const boost::uint64_t time = 213123;

    EXPECT_FALSE(meta.hasUserTime(key));
    meta.setUserTime(key, time);
    EXPECT_TRUE(meta.hasUserTime(key));
    EXPECT_EQ(time, meta.getUserTime(key));
    const boost::uint64_t updatedTime = 634545;
    meta.setUserTime(key, updatedTime);
    EXPECT_EQ(updatedTime, meta.getUserTime(key));

    EXPECT_EQ(size_t(1), meta.userTimeKeys().size());
    EXPECT_TRUE(meta.userTimeKeys().count(key) > 0);

    EXPECT_THROW(meta.getUserTime("unknown"), invalid_argument);

    const string autoKey = "auto";
    meta.setUserTime(autoKey);

    EXPECT_GT(meta.getUserTime(autoKey), rsc::misc::currentTimeMicros() - 100000);
    EXPECT_LE(meta.getUserTime(autoKey), rsc::misc::currentTimeMicros());

}

TEST(MetaDataTest, testComparison)
{

    MetaData meta1;
    MetaData meta2;
    meta2.setEventCreationTime(meta1.getEventCreationTime());
    EXPECT_EQ(meta1, meta2);

    meta1.setSenderId(rsc::misc::UUID());
    EXPECT_NE(meta1, meta2);
    meta2.setSenderId(meta1.getSenderId());
    EXPECT_EQ(meta1, meta2);

}
