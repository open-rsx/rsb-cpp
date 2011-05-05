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

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsc/misc/langutils.h>
#include <rsc/misc/UUID.h>

#include "rsb/transport/spread/Assembly.h"
#include "rsb/protocol/Notification.pb.h"

#include "../../InformerTask.h"

#include "testhelpers.h"
#include "testconfig.h"

using namespace std;
using namespace rsb;
using namespace rsb::test;
using namespace rsb::transport;
using namespace rsb::spread;
using namespace testing;
using namespace rsc::threading;

TEST(AssemblyTest, testAssembly)
{

    stringstream containedData;

    protocol::NotificationPtr initialNotification(new protocol::Notification);

    const string id = rsc::misc::UUID().getIdAsString();
    initialNotification->set_id(id);

    const string initialString = rsc::misc::randAlnumStr(10);
    initialNotification->mutable_data()->set_binary(initialString);
    containedData << initialString;

    const unsigned int dataParts = 5;
    initialNotification->set_num_data_parts(dataParts);
    initialNotification->set_data_part(0);
    Assembly assembly(initialNotification);
    EXPECT_FALSE(assembly.isComplete());

    for(unsigned int i = 1; i < dataParts; ++i) {

        protocol::NotificationPtr newNotification(new protocol::Notification);

        const string newString = rsc::misc::randAlnumStr(30);
        newNotification->mutable_data()->set_binary(newString);
        containedData << newString;

        newNotification->set_id(id);
        newNotification->set_num_data_parts(dataParts);
        newNotification->set_data_part(i);

        assembly.add(newNotification);

        if (i == (dataParts - 1)) {
            EXPECT_TRUE(assembly.isComplete());
            EXPECT_EQ(containedData.str(), *(assembly.getCompleteData()));
        } else {
            EXPECT_FALSE(assembly.isComplete());
        }

    }

}

TEST(AssemblyTest, testAge)
{

    protocol::NotificationPtr initialNotification(new protocol::Notification);

    const string id = rsc::misc::UUID().getIdAsString();
    initialNotification->set_id(id);

    const string initialString = rsc::misc::randAlnumStr(10);
    initialNotification->mutable_data()->set_binary(initialString);

    const unsigned int dataParts = 5;
    initialNotification->set_num_data_parts(dataParts);
    initialNotification->set_data_part(0);
    Assembly assembly(initialNotification);
    EXPECT_LT(assembly.age(), (unsigned int) 1);
    boost::this_thread::sleep(boost::posix_time::seconds(3));
    EXPECT_GE(assembly.age(), (unsigned int) 3);
    EXPECT_LE(assembly.age(), (unsigned int) 5);

}
