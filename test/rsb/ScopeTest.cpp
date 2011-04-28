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

#include "rsb/Scope.h"

using namespace std;
using namespace testing;
using namespace rsb;

TEST(ScopeTest, testParsing)
{

    Scope root("/");
    EXPECT_TRUE(root.getComponents().empty());

    Scope onePart("/test/");
    ASSERT_EQ(size_t(1), onePart.getComponents().size());
    EXPECT_EQ("test", onePart.getComponents()[0]);

    Scope manyParts("/this/is/a/dumb/test/");
    ASSERT_EQ(size_t(5), manyParts.getComponents().size());
    EXPECT_EQ("this", manyParts.getComponents()[0]);
    EXPECT_EQ("is", manyParts.getComponents()[1]);
    EXPECT_EQ("a", manyParts.getComponents()[2]);
    EXPECT_EQ("dumb", manyParts.getComponents()[3]);
    EXPECT_EQ("test", manyParts.getComponents()[4]);

    // also ensure that the shortcut syntax works
    Scope shortcut("/this/is");
    ASSERT_EQ(size_t(2), shortcut.getComponents().size());
    EXPECT_EQ("this", shortcut.getComponents()[0]);
    EXPECT_EQ("is", shortcut.getComponents()[1]);

}

TEST(ScopeTest, testParsingErrors)
{

    EXPECT_THROW(Scope(""), invalid_argument);
    EXPECT_THROW(Scope(" "), invalid_argument);
    EXPECT_THROW(Scope("/23"), invalid_argument);
    EXPECT_THROW(Scope("/with space/does/not/work/"), invalid_argument);
    EXPECT_THROW(Scope("/withnumbers/does/not43as/work/"), invalid_argument);
    EXPECT_THROW(Scope("/with/do#3es/not43as/work/"), invalid_argument);
    EXPECT_THROW(Scope("/this//is/not/allowed/"), invalid_argument);
    EXPECT_THROW(Scope("/this/ /is/not/allowed/"), invalid_argument);

}

TEST(ScopeTest, testToString)
{

    EXPECT_EQ("/", Scope("/").toString());
    EXPECT_EQ("/foo/", Scope("/foo/").toString());
    EXPECT_EQ("/foo/bar/", Scope("/foo/bar/").toString());
    // shortcut syntax
    EXPECT_EQ("/foo/bar/", Scope("/foo/bar").toString());

}

TEST(ScopeTest, testComparison)
{

    EXPECT_TRUE(Scope("/") == Scope("/"));
    EXPECT_FALSE(Scope("/") != Scope("/"));
    EXPECT_FALSE(Scope("/") == Scope("/foo/"));
    EXPECT_TRUE(Scope("/") != Scope("/foo/"));

    EXPECT_TRUE(Scope("/a/") < Scope("/c/"));
    EXPECT_TRUE(Scope("/a/") <= Scope("/c/"));
    EXPECT_TRUE(Scope("/a/") <= Scope("/a"));
    EXPECT_FALSE(Scope("/a/") > Scope("/c/"));
    EXPECT_TRUE(Scope("/c/") > Scope("/a/"));
    EXPECT_TRUE(Scope("/c/") >= Scope("/a/"));
    EXPECT_TRUE(Scope("/c/") >= Scope("/c/"));

}

TEST(ScopeTest, testConcat)
{

    EXPECT_EQ(Scope("/"), Scope("/").concat(Scope("/")));
    EXPECT_EQ(Scope("/a/test/"), Scope("/").concat(Scope("/a/test/")));
    EXPECT_EQ(Scope("/a/test/"), Scope("/a/test/").concat(Scope("/")));
    EXPECT_EQ(Scope("/a/test/example"), Scope("/a/test/").concat(Scope("/example/")));

}

TEST(ScopeTest, testHierarchyComparison)
{

    EXPECT_TRUE(Scope("/a/").isSubScopeOf(Scope("/")));
    EXPECT_TRUE(Scope("/a/b/c/").isSubScopeOf(Scope("/")));
    EXPECT_TRUE(Scope("/a/b/c/").isSubScopeOf(Scope("/a/b/")));
    EXPECT_FALSE(Scope("/a/b/c/").isSubScopeOf(Scope("/a/b/c/")));
    EXPECT_FALSE(Scope("/a/b/c/").isSubScopeOf(Scope("/a/b/c/d/")));
    EXPECT_FALSE(Scope("/a/x/c/").isSubScopeOf(Scope("/a/b/")));

    EXPECT_TRUE(Scope("/").isSuperScopeOf(Scope("/a/")));
    EXPECT_TRUE(Scope("/").isSuperScopeOf(Scope("/a/b/c/")));
    EXPECT_TRUE(Scope("/a/b/").isSuperScopeOf(Scope("/a/b/c/")));
    EXPECT_FALSE(Scope("/a/b/c/").isSuperScopeOf(Scope("/a/b/c/")));
    EXPECT_FALSE(Scope("/a/b/c/d/").isSuperScopeOf(Scope("/a/b/c/")));
    EXPECT_FALSE(Scope("/b/").isSuperScopeOf(Scope("/a/b/c/")));

}
