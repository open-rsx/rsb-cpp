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
