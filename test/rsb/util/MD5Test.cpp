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

#include "rsb/util/MD5.h"

using namespace std;
using namespace testing;
using namespace rsb;
using namespace rsb::util;

TEST(MD5Test, testConstruct)
{
    MD5("");
    MD5("   sdä#säf#säf#ŋđ↓ŋſđðđ ");
}

TEST(MD5Test, testHexString)
{
    {
        MD5 md5("The quick brown fox jumps over the lazy dog");
        EXPECT_EQ("9e107d9d372bb6826bd81d3542a419d6", md5.toHexString());
        EXPECT_EQ("9e107d9d 372bb682 6bd81d35 42a419d6", md5.toHexString(true));
    }

    {
        MD5 md5("/foo/bar/");
        EXPECT_EQ("1c184f3891344400380281315d9e7388", md5.toHexString());
        EXPECT_EQ("1c184f38 91344400 38028131 5d9e7388", md5.toHexString(true));
    }
}
