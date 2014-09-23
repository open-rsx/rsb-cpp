/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2014 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <rsc/runtime/TypeStringTools.h>

#include "rsb/Scope.h"

#include "rsb/filter/TypeFilter.h"

using namespace rsb;
using namespace rsb::filter;

using namespace testing;

class TypeFilterTest: public ::testing::Test {
};

TEST(TypeFilterTest, testMatch)
{

    boost::shared_ptr<std::string> payload(new std::string("foo"));

    {
        TypeFilter filter(rsc::runtime::typeName<std::string>());
        EXPECT_TRUE(filter.match(EventPtr(new Event(Scope("/"), payload,
                                                    rsc::runtime::typeName<std::string>()))));
    }

    {
        TypeFilter filter(rsc::runtime::typeName<std::string>(), true);
        EXPECT_FALSE(filter.match(EventPtr(new Event(Scope("/"), payload,
                                                     rsc::runtime::typeName<std::string>()))));
    }

    {
        boost::shared_ptr<TypeFilter> filter(TypeFilter::createForType<std::string>());
        EXPECT_TRUE(filter->match(EventPtr(new Event(Scope("/"), payload,
                                                     rsc::runtime::typeName<std::string>()))));
    }

    {
        boost::shared_ptr<TypeFilter> filter(TypeFilter::createForType<std::string>(true));
        EXPECT_FALSE(filter->match(EventPtr(new Event(Scope("/"), payload,
                                                      rsc::runtime::typeName<std::string>()))));
    }

}
