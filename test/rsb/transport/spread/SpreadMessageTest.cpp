/* ============================================================
 *
 * This file is a part of the RSB project
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

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsc/misc/langutils.h>
#include <rsc/misc/UUID.h>

#include "rsb/transport/spread/SpreadMessage.h"

using namespace std;
using namespace rsb;
using namespace rsb::transport::spread;
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
