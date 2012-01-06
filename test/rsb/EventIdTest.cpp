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
