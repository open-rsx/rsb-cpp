/* ============================================================
 *
 * This file is a part of RSB project
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

#include <stdexcept>

#include <boost/format.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/ParticipantConfig.h"

#include "testconfig.h"

using namespace std;
using namespace boost;
using namespace testing;
using namespace rsb;

TEST(TransportTest, testCreation)
{

    EXPECT_THROW(ParticipantConfig::Transport(""), invalid_argument);
    EXPECT_EQ("a test", ParticipantConfig::Transport("a test").getName());
    EXPECT_TRUE(ParticipantConfig::Transport("a test").getOptions().empty());

}

TEST(TransportTest, testOptions)
{

    ParticipantConfig::Transport transport("a test");
    rsc::runtime::Properties options;
    options.insert(make_pair("key", "value"));
    EXPECT_FALSE(transport.getOptions().has("key"));
    transport.setOptions(options);
    EXPECT_TRUE(transport.getOptions().has("key"));

}

TEST(EventProcessingStrategyOptionTest, testMutableOptions)
{
    ParticipantConfig::EventProcessingStrategy ep("foo");
    const string key = "foo";
    const string value = "bar";
    ep.mutableOptions().insert(make_pair(key, value));
    EXPECT_EQ(value, ep.getOptions().get<string> (key));
}

TEST(ParticipantConfigTest, testCreation)
{

    ParticipantConfig config;
    EXPECT_TRUE(config.getTransports().empty());
    EXPECT_EQ(QualityOfServiceSpec(), config.getQualityOfServiceSpec());
    EXPECT_EQ(ParticipantConfig::LOG, config.getErrorStrategy());

}

TEST(ParticipantConfigTest, testQualityOfServiceSpec)
{

    ParticipantConfig config;

    QualityOfServiceSpec spec(QualityOfServiceSpec::ORDERED,
            QualityOfServiceSpec::UNRELIABLE);
    EXPECT_NE(spec, config.getQualityOfServiceSpec());
    config.setQualityOfServiceSpec(spec);
    EXPECT_EQ(spec, config.getQualityOfServiceSpec());

}

TEST(ParticipantConfigTest, testTransports)
{

    ParticipantConfig config;

    ParticipantConfig::Transport t1("t1");
    ParticipantConfig::Transport t2("t2");

    EXPECT_EQ(size_t(0), config.getTransports().size());
    config.addTransport(t1);
    EXPECT_EQ(size_t(1), config.getTransports().size());
    config.addTransport(t1);
    EXPECT_EQ(size_t(1), config.getTransports().size());
    config.addTransport(t2);
    EXPECT_EQ(size_t(2), config.getTransports().size());

    config.removeTransport(ParticipantConfig::Transport("unknown"));
    EXPECT_EQ(size_t(2), config.getTransports().size());
    config.removeTransport(ParticipantConfig::Transport(t1.getName()));
    EXPECT_EQ(size_t(1), config.getTransports().size());
    config.removeTransport(ParticipantConfig::Transport(t2));
    EXPECT_EQ(size_t(0), config.getTransports().size());

    set<ParticipantConfig::Transport> newSet;
    newSet.insert(ParticipantConfig::Transport("foo"));
    config.setTransports(newSet);
    EXPECT_EQ(newSet, config.getTransports());

}

TEST(ParticipantConfigTest, testMutableTransport)
{

    ParticipantConfig config;

    const string name = "t1";
    ParticipantConfig::Transport t1(name);
    t1.setEnabled(false);
    config.addTransport(t1);
    config.mutableTransport(name).setEnabled(true);
    EXPECT_TRUE(config.getTransport(name).isEnabled());

}

TEST(ParticipantConfigTest, testOptions)
{

    ParticipantConfig config;
    rsc::runtime::Properties options;
    options.insert(make_pair("key", "value"));
    EXPECT_FALSE(config.getOptions().has("key"));
    config.setOptions(options);
    EXPECT_TRUE(config.getOptions().has("key"));

}

TEST(ParticipantConfigTest, testMutableOptions)
{

    ParticipantConfig config;
    const string key = "foo";
    const string value = "bar";
    config.mutableOptions().insert(make_pair(key, value));
    EXPECT_EQ(value, config.getOptions().get<string> (key));

}

TEST(ParticipantConfigTest, testFromFile)
{
    ParticipantConfig config = ParticipantConfig::fromFile(str(format(
            "%1%/rsb.conf-for-smoke-test") % TEST_ROOT));
    EXPECT_EQ(config.getOptions().getAs<unsigned int>("global"), 1u);

    EXPECT_EQ(config.getQualityOfServiceSpec().getReliability(),
            QualityOfServiceSpec::UNRELIABLE);
    EXPECT_EQ(config.getQualityOfServiceSpec().getOrdering(),
            QualityOfServiceSpec::UNORDERED);

    EXPECT_EQ(config.getErrorStrategy(), ParticipantConfig::EXIT);

    ParticipantConfig::Transport spread = config.getTransport("spread");
    EXPECT_EQ(spread.getOptions().get<string>("host"), "localhost");
    EXPECT_EQ(spread.getOptions().getAs<unsigned int>("port"), 4803u);

    EXPECT_EQ(spread.getConverters().size(), 1u);
    EXPECT_EQ(spread.getConverters().begin()->first, "image");
    EXPECT_EQ(spread.getConverters().begin()->second, "IplImage");

    for (unsigned int i = 3; i <= 6; ++i) {
        EXPECT_THROW(ParticipantConfig::fromFile(str(format("%1%/rsb.conf-semantic-errors-%2%")
                                % TEST_ROOT % i)),
                invalid_argument);
    }
}

TEST(ParticipantConfigTest, testErrorStrategy)
{

    ParticipantConfig config;
    const ParticipantConfig::ErrorStrategy strategy = ParticipantConfig::EXIT;
    config.setErrorStrategy(strategy);
    EXPECT_EQ(strategy, config.getErrorStrategy());

}
