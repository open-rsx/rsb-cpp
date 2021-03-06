/* ============================================================
 *
 * This file is a part of RSB project
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

    Scope manyParts("/this/is/a/dumb3/test/");
    ASSERT_EQ(size_t(5), manyParts.getComponents().size());
    EXPECT_EQ("this", manyParts.getComponents()[0]);
    EXPECT_EQ("is", manyParts.getComponents()[1]);
    EXPECT_EQ("a", manyParts.getComponents()[2]);
    EXPECT_EQ("dumb3", manyParts.getComponents()[3]);
    EXPECT_EQ("test", manyParts.getComponents()[4]);

    // also ensure that the shortcut syntax works
    Scope shortcut("/this/is");
    ASSERT_EQ(size_t(2), shortcut.getComponents().size());
    EXPECT_EQ("this", shortcut.getComponents()[0]);
    EXPECT_EQ("is", shortcut.getComponents()[1]);

}

void implicitScopeTester(const Scope /*scope*/) {
}

TEST(ScopeTest, testImplicitConstruction)
{
    implicitScopeTester("/another/test");
}

TEST(ScopeTest, testParsingErrors)
{

    EXPECT_THROW(Scope(""), invalid_argument);
    EXPECT_THROW(Scope(" "), invalid_argument);
    EXPECT_THROW(Scope("/with space/does/not/work/"), invalid_argument);
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
    EXPECT_EQ("/", Scope("/").concat(Scope("/")).toString());

    EXPECT_EQ(Scope("/a/test/"), Scope("/").concat(Scope("/a/test/")));
    EXPECT_EQ("/a/test/", Scope("/").concat(Scope("/a/test/")).toString());

    EXPECT_EQ(Scope("/a/test/"), Scope("/a/test/").concat(Scope("/")));
    EXPECT_EQ("/a/test/", Scope("/a/test/").concat(Scope("/")).toString());

    // Test concat with warm string caches.
    EXPECT_EQ(Scope("/a/test/example"), Scope("/a/test/").concat(Scope("/example/")));
    Scope left("/a/test/");
    left.toString();
    Scope right("/example/");
    right.toString();
    EXPECT_EQ("/a/test/example/", left.concat(right).toString());

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

TEST(ScopeTest, testSuperScopes)
{

    EXPECT_TRUE(Scope("/").superScopes().empty());

    vector<Scope> supers = Scope("/this/is/a/test/").superScopes();
    ASSERT_EQ(size_t(4), supers.size());
    EXPECT_EQ(Scope("/"), supers[0]);
    EXPECT_EQ(Scope("/this/"), supers[1]);
    EXPECT_EQ(Scope("/this/is/"), supers[2]);
    EXPECT_EQ(Scope("/this/is/a/"), supers[3]);

    supers = Scope("/").superScopes(true);
    ASSERT_EQ(size_t(1), supers.size());
    EXPECT_EQ(Scope("/"), supers[0]);

    supers = Scope("/this/is/a/test/").superScopes(true);
    ASSERT_EQ(size_t(5), supers.size());
    EXPECT_EQ(Scope("/"), supers[0]);
    EXPECT_EQ(Scope("/this/"), supers[1]);
    EXPECT_EQ(Scope("/this/is/"), supers[2]);
    EXPECT_EQ(Scope("/this/is/a/"), supers[3]);
    EXPECT_EQ(Scope("/this/is/a/test/"), supers[4]);

}

TEST(ScopeTest, testRootScopeConstruction)
{
    EXPECT_EQ(Scope(), Scope("/"));
    EXPECT_TRUE(Scope().getComponents().empty());
    EXPECT_TRUE(Scope("/").getComponents().empty());
}
