/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "rsb/Handler.h"

#include <iostream>

#include <boost/bind.hpp>

#include <rsc/runtime/TypeStringTools.h>
#include <rsc/misc/langutils.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/converter/BoolConverter.h"
#include "rsb/converter/StringConverter.h"
#include "rsb/converter/Uint64Converter.h"
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
	string schema =
			c.serialize(
					make_pair(
							rsc::runtime::typeName<bool>(),
							boost::shared_ptr<void>(&expected,
									rsc::misc::NullDeleter())), wire);
	AnnotatedData result = c.deserialize(schema, wire);
	EXPECT_EQ(expected, *(boost::static_pointer_cast<bool>(result.second)));

}

INSTANTIATE_TEST_CASE_P(DefaultConverterTest, BoolConverterTest,
		::testing::Bool())
;

class StringConverterTest: public ::testing::TestWithParam<string> {
};

TEST_P(StringConverterTest, testRoundtrip)
{

	StringConverter c;
	string wire;
	string expected = GetParam();
	string schema =
			c.serialize(
					make_pair(
							rsc::runtime::typeName<string>(),
							boost::shared_ptr<void>(&expected,
									rsc::misc::NullDeleter())), wire);
	AnnotatedData result = c.deserialize(schema, wire);
	EXPECT_EQ(expected, *(boost::static_pointer_cast<string>(result.second)));

}

INSTANTIATE_TEST_CASE_P(DefaultConverterTest, StringConverterTest,
		::testing::Values("", "hello", " with space   inside ", "    %&$%&§$ſŧ←ðħſ"))
;

class Uint64ConverterTest: public ::testing::TestWithParam<boost::uint64_t> {
};

TEST_P(Uint64ConverterTest, testRoundtrip)
{

	Uint64Converter c;
	string wire;
	boost::uint64_t expected = GetParam();
	string schema =
			c.serialize(
					make_pair(
							rsc::runtime::typeName<boost::uint64_t>(),
							boost::shared_ptr<void>(&expected,
									rsc::misc::NullDeleter())), wire);
	AnnotatedData result = c.deserialize(schema, wire);
	EXPECT_EQ(expected, *(boost::static_pointer_cast<boost::uint64_t>(result.second)));

}

INSTANTIATE_TEST_CASE_P(DefaultConverterTest, Uint64ConverterTest,
		::testing::Values<boost::uint64_t>(0, 1, 12342423439))
;

TEST(VoidConverterTest, testRoundtrip)
{

	VoidConverter c;
	string wire;
	string schema =
			c.serialize(
					make_pair(rsc::runtime::typeName<void>(),
							boost::shared_ptr<void>()), wire);
	AnnotatedData result = c.deserialize(schema, wire);
	EXPECT_EQ(rsc::runtime::typeName<void>(), result.first);

}
