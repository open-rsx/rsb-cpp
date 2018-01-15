/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <string>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/eventprocessing/ScopeDispatcher.h"

using namespace testing;

using namespace rsb::eventprocessing;

TEST(ScopeDispatcherTest, testAdd)
{

    ScopeDispatcher<std::string> dispatcher;
    EXPECT_TRUE(dispatcher.empty());
    EXPECT_EQ(dispatcher.size(), 0);

    dispatcher.addSink("/foo", "bar");
    EXPECT_FALSE(dispatcher.empty());
    EXPECT_EQ(dispatcher.size(), 1);

    dispatcher.addSink("/foo", "baz");
    EXPECT_FALSE(dispatcher.empty());
    EXPECT_EQ(dispatcher.size(), 1);

    dispatcher.addSink("/fez", "whoop");
    EXPECT_FALSE(dispatcher.empty());
    EXPECT_EQ(dispatcher.size(), 2);
}

TEST(ScopeDispatcherTest, testRemove)
{

    ScopeDispatcher<std::string> dispatcher;
    dispatcher.addSink("/foo", "bar");
    dispatcher.addSink("/foo", "baz");

    dispatcher.removeSink("/foo", "bar");
    EXPECT_FALSE(dispatcher.empty());
    EXPECT_EQ(dispatcher.size(), 1);

    dispatcher.removeSink("/foo", "baz");
    EXPECT_TRUE(dispatcher.empty());
    EXPECT_EQ(dispatcher.size(), 0);
}

/* Only works on some compilers. Whatever. It's C++ after all.
TEST(ScopeDispatcherTest, testMapSinks)
{
    ScopeDispatcher<std::string> dispatcher;
    dispatcher.addSink("/foo",     "bar");
    dispatcher.addSink("/foo/fez", "baz");
    dispatcher.addSink("/whoop",   "di");

    struct PoorPersonsLambda {
        void operator()(const std::string& sink) {
            this->calls.push_back(sink);
        }

        std::vector<std::string> calls;
    };
    PoorPersonsLambda lambda;
    dispatcher.mapSinks("/foo", boost::ref(lambda));
    EXPECT_EQ(lambda.calls.size(), 1);
    EXPECT_EQ(lambda.calls[0], "bar");
    }*/


TEST(WeakScopeDispatcherTest, testAdd)
{

    WeakScopeDispatcher<std::string> dispatcher;
    EXPECT_TRUE(dispatcher.empty());
    EXPECT_EQ(dispatcher.size(), 0);

    boost::shared_ptr<std::string> bar(new std::string("bar"));
    dispatcher.addSink("/foo", bar);
    EXPECT_FALSE(dispatcher.empty());
    EXPECT_EQ(dispatcher.size(), 1);

    boost::shared_ptr<std::string> baz(new std::string("baz"));
    dispatcher.addSink("/foo", baz);
    EXPECT_FALSE(dispatcher.empty());
    EXPECT_EQ(dispatcher.size(), 1);

    boost::shared_ptr<std::string> whoop(new std::string("whoop"));
    dispatcher.addSink("/fez", whoop);
    EXPECT_FALSE(dispatcher.empty());
    EXPECT_EQ(dispatcher.size(), 2);
}

TEST(WeakScopeDispatcherTest, testRemove)
{

    WeakScopeDispatcher<std::string> dispatcher;
    boost::shared_ptr<std::string> bar(new std::string("bar"));
    dispatcher.addSink("/foo", bar);
    boost::shared_ptr<std::string> baz(new std::string("baz"));
    dispatcher.addSink("/foo", baz);
    boost::shared_ptr<std::string> whoop(new std::string("whoop"));
    dispatcher.addSink("/fez", whoop);

    dispatcher.removeSink("/fez", whoop.get());
    EXPECT_FALSE(dispatcher.empty());
    EXPECT_EQ(dispatcher.size(), 1);

    // With baz dead and bar explicitly removed, there should be no
    // remaining entries for scope /foo.
    baz.reset();
    dispatcher.removeSink("/foo", bar.get());
    EXPECT_TRUE(dispatcher.empty());
    EXPECT_EQ(dispatcher.size(), 0);
}

/* See above for helpful, non-jaded explanatory comment.
TEST(WeakScopeDispatcherTest, testMapSinks)
{
    WeakScopeDispatcher<std::string> dispatcher;
    boost::shared_ptr<std::string> bar(new std::string("bar"));
    dispatcher.addSink("/foo",     bar);
    boost::shared_ptr<std::string> baz(new std::string("baz"));
    dispatcher.addSink("/foo/fez", baz);
    boost::shared_ptr<std::string> di(new std::string("di"));
    dispatcher.addSink("/whoop",   di);

    struct PoorPersonsLambda {
        void operator()(const std::string& sink) {
            this->calls.push_back(sink);
        }

        std::vector<std::string> calls;
    };
    PoorPersonsLambda lambda;
    dispatcher.mapSinks("/foo", boost::ref(lambda));
    EXPECT_EQ(lambda.calls.size(), 1);
    EXPECT_EQ(lambda.calls[0], "bar");
    }*/
