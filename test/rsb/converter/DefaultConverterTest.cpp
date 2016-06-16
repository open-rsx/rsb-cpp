/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2013, 2016 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "rsb/Handler.h"

#include <iostream>
#include <limits>

#include <boost/bind.hpp>

#include <rsc/runtime/TypeStringTools.h>
#include <rsc/misc/langutils.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/converter/BoolConverter.h"
#include "rsb/converter/ByteArrayConverter.h"
#include "rsb/converter/StringConverter.h"
#include "rsb/converter/IntegerConverter.h"
#include "rsb/converter/FloatingPointConverter.h"
#include "rsb/converter/VoidConverter.h"

using namespace std;

using namespace rsb;
using namespace rsb::converter;

using namespace testing;

class BoolConverterTest: public ::testing::TestWithParam<bool> {
};

TEST_P(BoolConverterTest, testRoundtrip)
{

    BoolConverter c;
    string wire;
    bool expected = GetParam();
    string schema
        = c.serialize(make_pair(rsc::runtime::typeName<bool>(),
                                boost::shared_ptr<void>(&expected, rsc::misc::NullDeleter())),
                      wire);
    AnnotatedData result = c.deserialize(schema, wire);
    EXPECT_EQ(expected, *(boost::static_pointer_cast<bool>(result.second)));

}

INSTANTIATE_TEST_CASE_P(DefaultConverterTest, BoolConverterTest,
                        ::testing::Bool());

class StringConverterTest: public ::testing::TestWithParam<string> {
};

TEST_P(StringConverterTest, testRoundtrip)
{

    StringConverter c;
    string wire;
    string expected = GetParam();
    string schema
        = c.serialize(make_pair(rsc::runtime::typeName<string>(),
                                boost::shared_ptr<void>(&expected, rsc::misc::NullDeleter())),
                      wire);
    AnnotatedData result = c.deserialize(schema, wire);
    EXPECT_EQ(expected, *(boost::static_pointer_cast<string>(result.second)));

}

INSTANTIATE_TEST_CASE_P(DefaultConverterTest, StringConverterTest,
                        ::testing::Values("", "hello", " with space   inside ", "    %&$%&§$ſŧ←ðħſ"));

class Uint32ConverterTest: public ::testing::TestWithParam<boost::uint32_t> {
};

TEST_P(Uint32ConverterTest, testRoundtrip)
{

    Uint32Converter c;
    string wire;
    boost::uint32_t expected = GetParam();
    string schema
        = c.serialize(make_pair(rsc::runtime::typeName<boost::uint32_t>(),
                                boost::shared_ptr<void>(&expected, rsc::misc::NullDeleter())),
                      wire);
    AnnotatedData result = c.deserialize(schema, wire);
    EXPECT_EQ(expected, *(boost::static_pointer_cast<boost::uint32_t>(result.second)));

}

INSTANTIATE_TEST_CASE_P(DefaultConverterTest, Uint32ConverterTest,
                        ::testing::Values<boost::uint32_t>(0, 1, 1234242));

class Uint64ConverterTest: public ::testing::TestWithParam<boost::uint64_t> {
};

TEST_P(Uint64ConverterTest, testRoundtrip)
{

    Uint64Converter c;
    string wire;
    boost::uint64_t expected = GetParam();
    string schema
        = c.serialize(make_pair(rsc::runtime::typeName<boost::uint64_t>(),
                                boost::shared_ptr<void>(&expected, rsc::misc::NullDeleter())),
                      wire);
    AnnotatedData result = c.deserialize(schema, wire);
    EXPECT_EQ(expected, *(boost::static_pointer_cast<boost::uint64_t>(result.second)));

}

INSTANTIATE_TEST_CASE_P(DefaultConverterTest, Uint64ConverterTest,
                        ::testing::Values<boost::uint64_t>(0, 1, 12342423439));

class Int64ConverterTest: public ::testing::TestWithParam<boost::int64_t> {
};

TEST_P(Int64ConverterTest, testRoundtrip)
{

    Int64Converter c;
    string wire;
    boost::int64_t expected = GetParam();
    string schema
        = c.serialize(make_pair(rsc::runtime::typeName<boost::int64_t>(),
                                boost::shared_ptr<void>(&expected, rsc::misc::NullDeleter())),
                      wire);
    AnnotatedData result = c.deserialize(schema, wire);
    EXPECT_EQ(expected, *(boost::static_pointer_cast<boost::int64_t>(result.second)));

}

INSTANTIATE_TEST_CASE_P(DefaultConverterTest, Int64ConverterTest,
                        ::testing::Values<boost::int64_t>(0, 1, 12342423439, -1, -12342423439));

class Int32ConverterTest: public ::testing::TestWithParam<boost::int32_t> {
};

TEST_P(Int32ConverterTest, testRoundtrip)
{

    Int32Converter c;
    string wire;
    boost::int32_t expected = GetParam();
    string schema
        = c.serialize(make_pair(rsc::runtime::typeName<boost::int32_t>(),
                                boost::shared_ptr<void>(&expected, rsc::misc::NullDeleter())),
                      wire);
    AnnotatedData result = c.deserialize(schema, wire);
    EXPECT_EQ(expected, *(boost::static_pointer_cast<boost::int32_t>(result.second)));

}

INSTANTIATE_TEST_CASE_P(DefaultConverterTest, Int32ConverterTest,
                        ::testing::Values<boost::int32_t>(0, 1, 12342423439, -1, -12342423439));

class FloatConverterTest: public ::testing::TestWithParam<float> {
};

TEST_P(FloatConverterTest, testRoundtrip)
{

    FloatConverter c;
    string wire;
    float expected = GetParam();
    string schema
        = c.serialize(make_pair(rsc::runtime::typeName<float>(),
                                boost::shared_ptr<void>(&expected, rsc::misc::NullDeleter())),
                      wire);
    AnnotatedData result = c.deserialize(schema, wire);
    EXPECT_EQ(expected, *(boost::static_pointer_cast<float>(result.second)));

}

INSTANTIATE_TEST_CASE_P(DefaultConverterTest, FloatConverterTest,
                        ::testing::Values<float>(0.0, 1.0, 1.5, 1e9,
                                                 -0.0, -1.0, -1.5, -1e9,
                                                 std::numeric_limits<float>::infinity(),
                                                 std::numeric_limits<float>::min(),
                                                 // C++11
                                                 //std::numeric_limits<float>::lowest(),
                                                 std::numeric_limits<float>::max()));

class DoubleConverterTest: public ::testing::TestWithParam<double> {
};

TEST_P(DoubleConverterTest, testRoundtrip)
{

    DoubleConverter c;
    string wire;
    double expected = GetParam();
    string schema
        = c.serialize(make_pair(rsc::runtime::typeName<double>(),
                                boost::shared_ptr<void>(&expected, rsc::misc::NullDeleter())),
                      wire);
    AnnotatedData result = c.deserialize(schema, wire);
    EXPECT_EQ(expected, *(boost::static_pointer_cast<double>(result.second)));

}

INSTANTIATE_TEST_CASE_P(DefaultConverterTest, DoubleConverterTest,
                        ::testing::Values<double>(0.0, 1.0, 1.5, 1e9,
                                                  -0.0, -1.0, -1.5, -1e9,
                                                  std::numeric_limits<double>::infinity(),
                                                  std::numeric_limits<double>::min(),
                                                  // C++11
                                                  //std::numeric_limits<double>::lowest(),
                                                  std::numeric_limits<double>::max()));

TEST(VoidConverterTest, testRoundtrip)
{

    VoidConverter c;
    string wire;
    string schema
        = c.serialize(make_pair(rsc::runtime::typeName<void>(),
                                boost::shared_ptr<void>()),
                      wire);
    AnnotatedData result = c.deserialize(schema, wire);
    EXPECT_EQ(rsc::runtime::typeName<void>(), result.first);

}

TEST(ByteArrayConverterTest, testDeserializeEmpty)
{

    ByteArrayConverter c;
    string wire;
    AnnotatedData result = c.deserialize("", wire);
    EXPECT_EQ(wire, *(boost::static_pointer_cast<string>(result.second)));

}

TEST(ByteArrayConverterTest, testDeserializeSimpleString)
{

    ByteArrayConverter c;
    string wire = "hallo welt!\nnew line\r\nfoo";
    AnnotatedData result = c.deserialize("", wire);
    EXPECT_EQ(wire, *(boost::static_pointer_cast<string>(result.second)));

}

TEST(ByteArrayConverterTest, testDeserializeNullBytes)
{

    ByteArrayConverter c;
    string wire = "hallo welt!\nnew line\r\nfoo\0withnull";
    AnnotatedData result = c.deserialize("", wire);
    EXPECT_EQ(wire, *(boost::static_pointer_cast<string>(result.second)));

}
