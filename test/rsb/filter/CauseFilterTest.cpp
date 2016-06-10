/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2014, 2016 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <boost/shared_ptr.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsc/misc/UUID.h>
#include <rsc/runtime/TypeStringTools.h>

#include "rsb/Scope.h"

#include "rsb/filter/CauseFilter.h"

using namespace rsb;
using namespace rsb::filter;

using namespace testing;

class CauseFilterTest: public ::testing::Test {
};

TEST(CauseFilterTest, testMatch)
{

    boost::shared_ptr<std::string> payload(new std::string("foo"));

    {
        CauseFilter filter = CauseFilter(EventId(rsc::misc::UUID(false), 0));
        EventPtr event(new Event());
        event->addCause(EventId(rsc::misc::UUID(false), 0));
        EXPECT_TRUE(filter.match(event));
    }

    {
        CauseFilter filter = CauseFilter(EventId(rsc::misc::UUID(false), 0),
                                         true);
        EventPtr event(new Event());
        event->addCause(EventId(rsc::misc::UUID(false), 0));
        EXPECT_FALSE(filter.match(event));
    }

    {
        CauseFilter filter = CauseFilter(EventId(rsc::misc::UUID(false), 0)
                                         .getAsUUID());
        EventPtr event(new Event());
        event->addCause(EventId(rsc::misc::UUID(false), 0));
        EXPECT_TRUE(filter.match(event));
    }

}
