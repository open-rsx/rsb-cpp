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
