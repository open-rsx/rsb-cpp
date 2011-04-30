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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/ParticipantConfig.h"
#include "rsb/Factory.h"
#include "rsb/LocalService.h"

using namespace std;
using namespace testing;
using namespace rsb;

/**
 * Test fixture for rsb::Factory.
 *
 * @author jwienke
 */
class FactoryTest: public ::testing::Test {
protected:

    virtual void SetUp() {
        Factory::killInstance();
    }

};


TEST_F(FactoryTest, testDefaultParticipantConfig)
{

    Factory &f = Factory::getInstance();

    EXPECT_EQ(size_t(1), f.getDefaultParticipantConfig().getTransports().size());

    ParticipantConfig config;
    config.addTransport(ParticipantConfig::Transport("foo"));
    config.addTransport(ParticipantConfig::Transport("bar"));

    f.setDefaultParticipantConfig(config);
    EXPECT_EQ(size_t(2), f.getDefaultParticipantConfig().getTransports().size());

}

TEST_F(FactoryTest, testCreateService)
{

    Factory &f = Factory::getInstance();

    Scope scopeA("/a");
    Scope scopeB("/b/with/sub");

    ServicePtr sA = f.createService(scopeA);
    ServicePtr sB = f.createService(scopeB);

    EXPECT_TRUE(sA);
    EXPECT_TRUE(sB);

    EXPECT_EQ(scopeA, sA->getScope());
    EXPECT_EQ(scopeB, sB->getScope());

    EXPECT_TRUE(boost::dynamic_pointer_cast<LocalService>(sA));
    EXPECT_TRUE(boost::dynamic_pointer_cast<LocalService>(sB));

}