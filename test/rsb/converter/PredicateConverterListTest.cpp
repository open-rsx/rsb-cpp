/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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
