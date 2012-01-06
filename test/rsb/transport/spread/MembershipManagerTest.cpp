/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/transport/spread/MembershipManager.h"

#include "testconfig.h"

using namespace std;
using namespace rsb::spread;
using namespace testing;

TEST(MembershipManagerTest, testRoundtrip)
{
    MembershipManagerPtr mm(new MembershipManager());
    // TODO convert this to a mock-only test case
    SpreadConnectionPtr sp(
            new SpreadConnection("blub", defaultHost(), SPREAD_PORT));
    sp->activate();

    ASSERT_NO_THROW(mm->join("a",sp));
    mm->join("a",sp);
    // join a different group
    mm->join("b",sp);
    // leave it
    mm->leave("b",sp);
    // leave a
    mm->leave("a",sp);
    mm->leave("a",sp);
    // re-join a previously left group
    mm->join("b",sp);
    // left b as well
    ASSERT_NO_THROW(mm->leave("b",sp));
}
