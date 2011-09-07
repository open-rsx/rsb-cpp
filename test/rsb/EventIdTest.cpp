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

#include <stdexcept>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/EventId.h"

using namespace std;
using namespace testing;
using namespace rsb;

TEST(EventIdTest, testEquality) {

    rsc::misc::UUID participantId;
    boost::uint32_t sequenceNumber = 121;

    EventId id1(participantId, sequenceNumber);
    EventId id2(participantId, sequenceNumber);
    EventId id3(participantId, sequenceNumber + 1);

    EXPECT_EQ(id1, id1);
    EXPECT_EQ(id1, id2);
    EXPECT_NE(id1, id3);

}

TEST(EventIdTest, testOrdering) {

    // same participant id
    rsc::misc::UUID sameId;
    boost::uint32_t sequenceNumber = 121;
    EventId reference(sameId, sequenceNumber);
    EventId bigger(sameId, sequenceNumber + 1);

    EXPECT_FALSE(reference < reference);
    EXPECT_TRUE(reference < bigger);
    EXPECT_FALSE(bigger < reference);

    // participant id already different
    rsc::misc::UUID id1;
    rsc::misc::UUID id2;
    EXPECT_NE(id1, id2);
    EventId eventId1(id1, sequenceNumber);
    EventId eventId2(id2, sequenceNumber);
    if (id1 < id2) {
        EXPECT_TRUE(eventId1 < eventId2);
    } else {
        EXPECT_TRUE(eventId2 < eventId1);
    }

}

TEST(EventIdTest, testUUID) {

    rsc::misc::UUID participantId;
    boost::uint32_t sequenceNumber = 121;

    EventId id1(participantId, sequenceNumber);
    EventId id2(participantId, sequenceNumber);

    EXPECT_EQ(id1.getAsUUID(), id2.getAsUUID());
    // repeated call to test caching
    EXPECT_EQ(id1.getAsUUID(), id2.getAsUUID());

}
