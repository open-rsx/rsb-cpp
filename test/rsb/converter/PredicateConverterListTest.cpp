/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <list>
#include <utility>

#include "rsb/converter/PredicateConverterList.h"
#include "rsb/converter/RegexConverterPredicate.h"
#include "rsb/converter/VoidConverter.h"

using namespace std;
using namespace rsb;
using namespace rsb::converter;
using namespace testing;

class PredicateConverterListTest: public ::testing::Test {
public:
    ConverterPredicatePtr p1;
    ConverterPredicatePtr p2;

    Converter<string>::Ptr v1;
    Converter<string>::Ptr v2;

    list< pair<ConverterPredicatePtr, Converter<string>::Ptr> > alwaysTrue;
    list< pair<ConverterPredicatePtr, Converter<string>::Ptr> > regex;
    list< pair<ConverterPredicatePtr, Converter<string>::Ptr> > mixed;

    void SetUp() {
        this->p1.reset(new AlwaysApplicable());
        this->p2.reset(new RegexConverterPredicate(".*foo.*"));

        this->v1.reset(new VoidConverter());
        this->v2.reset(new VoidConverter());

        this->alwaysTrue.push_back(make_pair(p1, v1));

        this->regex.push_back(make_pair(p2, v1));

        this->mixed.push_back(make_pair(p2, v1));
        this->mixed.push_back(make_pair(p1, v2));
    }
};

TEST_F(PredicateConverterListTest, testConstruction)
{
    PredicateConverterList<string>(alwaysTrue.begin(), alwaysTrue.end());
    PredicateConverterList<string>(regex.begin(), regex.end());
    PredicateConverterList<string>(mixed.begin(), mixed.end());
}

TEST_F(PredicateConverterListTest, testGetConverter)
{
    {
        PredicateConverterList<string> converters(alwaysTrue.begin(), alwaysTrue.end());
        EXPECT_EQ(converters.getConverter(""),    v1);
        EXPECT_EQ(converters.getConverter("bla"), v1);
    }

    {
        PredicateConverterList<string> converters(regex.begin(), regex.end());
        EXPECT_FALSE(converters.getConverter(""));
        EXPECT_FALSE(converters.getConverter("bla"));
        EXPECT_EQ   (converters.getConverter("foo"),    v1);
        EXPECT_EQ   (converters.getConverter("foobar"), v1);
    }

    {
        PredicateConverterList<string> converters(mixed.begin(), mixed.end());
        EXPECT_EQ(converters.getConverter(""),       v2);
        EXPECT_EQ(converters.getConverter("bla"),    v2);
        EXPECT_EQ(converters.getConverter("foo"),    v1);
        EXPECT_EQ(converters.getConverter("foobar"), v1);
    }
}
