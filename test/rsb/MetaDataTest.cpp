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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <stdexcept>

#include <rsc/misc/langutils.h>

#include "rsb/MetaData.h"

using namespace std;
using namespace testing;
using namespace rsb;

TEST(MetaDataTest, testConstruction) {

    MetaData meta;
    EXPECT_GT(meta.getCreateTime(), rsc::misc::currentTimeMicros() - 100000);
    EXPECT_LE(meta.getCreateTime(), rsc::misc::currentTimeMicros());
    EXPECT_EQ((boost::uint64_t) 0, meta.getReceiveTime());
    EXPECT_EQ((boost::uint64_t) 0, meta.getDeliverTime());
    EXPECT_EQ((boost::uint64_t) 0, meta.getSendTime());
    EXPECT_TRUE(meta.userTimeKeys().empty());

}

TEST(MetaDataTest, testSenderId) {
    const rsc::misc::UUID id;
    MetaData meta;
    meta.setSenderId(id);
    EXPECT_EQ(id, meta.getSenderId());
}

TEST(MetaDataTest, testTimesDouble) {

    // from the internal structure I assume that all double-based setters use
    // one common method. So one test is currently enough

    MetaData meta;

    double time = 123.24;
    meta.setCreateTime(time);
    EXPECT_EQ(boost::uint64_t(123240000), meta.getCreateTime());

}

TEST(MetaDataTest, testTimesPtime) {

    // from the internal structure I assume that all ptime-based setters use
    // one common method. So one test is currently enough

    MetaData meta;

    const boost::uint64_t before = rsc::misc::currentTimeMicros();
    const boost::posix_time::ptime time = boost::posix_time::microsec_clock::universal_time();
    const boost::uint64_t after = rsc::misc::currentTimeMicros();
    meta.setCreateTime(time);
    // it seems that on windows the two time methods use a slightly different
    // clock mechanism. Hence, we will allow a short offset for the comparison
    EXPECT_GE(meta.getCreateTime(), before - 2000);
    EXPECT_LE(meta.getCreateTime(), after + 2000);

}

TEST(MetaDataTest, testDefaultTimesAuto) {

    MetaData meta;
    meta.setCreateTime();
    meta.setSendTime();
    meta.setReceiveTime();
    meta.setDeliverTime();

    EXPECT_GT(meta.getCreateTime(), rsc::misc::currentTimeMicros() - 100000);
    EXPECT_LE(meta.getCreateTime(), rsc::misc::currentTimeMicros());
    EXPECT_GT(meta.getSendTime(), rsc::misc::currentTimeMicros() - 100000);
    EXPECT_LE(meta.getSendTime(), rsc::misc::currentTimeMicros());
    EXPECT_GT(meta.getReceiveTime(), rsc::misc::currentTimeMicros() - 100000);
    EXPECT_LE(meta.getReceiveTime(), rsc::misc::currentTimeMicros());
    EXPECT_GT(meta.getDeliverTime(), rsc::misc::currentTimeMicros() - 100000);
    EXPECT_LE(meta.getDeliverTime(), rsc::misc::currentTimeMicros());

}

TEST(MetaDataTest, testDefaultTimesManual) {

    MetaData meta;
    const boost::uint64_t time = 13123;
    meta.setCreateTime(time);
    meta.setSendTime(time);
    meta.setReceiveTime(time);
    meta.setDeliverTime(time);

    EXPECT_EQ(time, meta.getCreateTime());
    EXPECT_EQ(time, meta.getSendTime());
    EXPECT_EQ(time, meta.getReceiveTime());
    EXPECT_EQ(time, meta.getDeliverTime());

}

TEST(MetaDataTest, testUserTimes) {

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

    EXPECT_GT(meta.getUserTime(autoKey),
            rsc::misc::currentTimeMicros() - 100000);
    EXPECT_LE(meta.getUserTime(autoKey), rsc::misc::currentTimeMicros());

}

TEST(MetaDataTest, testUserInfos) {

    MetaData meta;
    const string key = "afdadfasfd";
    const string value = "213123";

    EXPECT_FALSE(meta.hasUserInfo(key));
    meta.setUserInfo(key, value);
    EXPECT_TRUE(meta.hasUserInfo(key));
    EXPECT_EQ(value, meta.getUserInfo(key));
    const string updatedInfo = "foobar";
    meta.setUserInfo(key, updatedInfo);
    EXPECT_EQ(updatedInfo, meta.getUserInfo(key));

    EXPECT_EQ(size_t(1), meta.userInfoKeys().size());
    EXPECT_TRUE(meta.userInfoKeys().count(key) > 0);

    EXPECT_THROW(meta.getUserInfo("unknown"), invalid_argument);

}

TEST(MetaDataTest, testComparison) {

    MetaData meta1;
    meta1.setSenderId(rsc::misc::UUID());
    MetaData meta2;
    meta2.setSenderId(rsc::misc::UUID());
    EXPECT_NE(meta1, meta2);
    // distinct times + UUIDs
    meta2.setCreateTime(meta1.getCreateTime());
    EXPECT_NE(meta1, meta2);
    // still distinct UUIDs

    meta2.setSenderId(meta1.getSenderId());
    EXPECT_EQ(meta1, meta2);
    // identical

    meta1.setSenderId(rsc::misc::UUID());
    EXPECT_NE(meta1, meta2);
    // distinct UUIDs, again

    meta2 = meta1;
    EXPECT_EQ(meta1, meta2);
    meta2.setUserInfo("foo", "bar");
    EXPECT_NE(meta1, meta2);
    meta1.setUserInfo("foo", "bar");
    EXPECT_EQ(meta1, meta2);

}
