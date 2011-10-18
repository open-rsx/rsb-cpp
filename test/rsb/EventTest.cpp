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

#include <time.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/Event.h"
#include "rsb/EventId.h"

using namespace std;
using namespace testing;
using namespace rsb;

TEST(EventTest, testCausality) {

    EventId cause1(rsc::misc::UUID(), rand());
    EventId cause2(rsc::misc::UUID(), rand());
    EventId cause3(rsc::misc::UUID(), rand());

    Event event;
    EXPECT_EQ(size_t(0), event.getCauses().size());
    EXPECT_FALSE(event.isCause(cause1));
    EXPECT_FALSE(event.isCause(cause2));
    EXPECT_FALSE(event.isCause(cause3));

    // add
    EXPECT_TRUE(event.addCause(cause1));
    EXPECT_TRUE(event.isCause(cause1));
    EXPECT_FALSE(event.isCause(cause2));
    EXPECT_FALSE(event.isCause(cause3));
    EXPECT_EQ(size_t(1), event.getCauses().size());
    EXPECT_TRUE(event.getCauses().count(cause1));
    EXPECT_FALSE(event.addCause(cause1));
    EXPECT_TRUE(event.isCause(cause1));

    // remove
    EXPECT_FALSE(event.removeCause(cause3));
    EXPECT_TRUE(event.isCause(cause1));
    EXPECT_FALSE(event.isCause(cause2));
    EXPECT_FALSE(event.isCause(cause3));
    EXPECT_EQ(size_t(1), event.getCauses().size());
    EXPECT_TRUE(event.getCauses().count(cause1));

    // add 2
    EXPECT_TRUE(event.addCause(cause2));
    EXPECT_FALSE(event.addCause(cause2));
    EXPECT_TRUE(event.isCause(cause1));
    EXPECT_TRUE(event.isCause(cause2));
    EXPECT_EQ(size_t(2), event.getCauses().size());

    // remove again
    EXPECT_TRUE(event.removeCause(cause1));
    EXPECT_EQ(size_t(1), event.getCauses().size());
    EXPECT_FALSE(event.removeCause(cause1));
    EXPECT_EQ(size_t(1), event.getCauses().size());

}
