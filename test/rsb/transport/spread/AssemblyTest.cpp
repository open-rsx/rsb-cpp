/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

using namespace testing;

using namespace rsc::threading;

using namespace rsb;
using namespace rsb::transport;
using namespace rsb::transport::spread;
using namespace rsb::test;

TEST(AssemblyTest, testAssembly) {

    stringstream containedData;

    protocol::FragmentedNotificationPtr initialNotification(
            new protocol::FragmentedNotification);

    boost::uint32_t seqnum = 0;
    initialNotification->mutable_notification()->mutable_event_id()->set_sequence_number(
            seqnum);

    const string initialString = rsc::misc::randAlnumStr(10);
    initialNotification->mutable_notification()->set_data(initialString);
    containedData << initialString;

    const unsigned int dataParts = 5;
    initialNotification->set_num_data_parts(dataParts);
    initialNotification->set_data_part(0);
    Assembly assembly(initialNotification);
    EXPECT_FALSE(assembly.isComplete());

    for (unsigned int i = 1; i < dataParts; ++i) {

        protocol::FragmentedNotificationPtr newNotification(
                new protocol::FragmentedNotification);

        const string newString = rsc::misc::randAlnumStr(30);
        newNotification->mutable_notification()->set_data(newString);
        containedData << newString;

        newNotification->mutable_notification()->mutable_event_id()->set_sequence_number(
                seqnum);
        newNotification->set_num_data_parts(dataParts);
        newNotification->set_data_part(i);

        bool complete = assembly.add(newNotification);

        if (i == (dataParts - 1)) {
            EXPECT_TRUE(assembly.isComplete());
            EXPECT_TRUE(complete);
            EXPECT_EQ(containedData.str(),
                    assembly.getCompleteNotification()->data());
        } else {
            EXPECT_FALSE(assembly.isComplete());
        }

    }

}

TEST(AssemblyTest, testAge) {

    protocol::FragmentedNotificationPtr initialNotification(
            new protocol::FragmentedNotification);

    boost::uint32_t seqnum = 0;
    initialNotification->mutable_notification()->mutable_event_id()->set_sequence_number(
            seqnum);

    const string initialString = rsc::misc::randAlnumStr(10);
    initialNotification->mutable_notification()->set_data(initialString);

    const unsigned int dataParts = 5;
    initialNotification->set_num_data_parts(dataParts);
    initialNotification->set_data_part(0);
    Assembly assembly(initialNotification);
    EXPECT_LT(assembly.age(), (unsigned int) 1);
    boost::this_thread::sleep(boost::posix_time::seconds(3));
    EXPECT_GE(assembly.age(), (unsigned int) 3);
    EXPECT_LE(assembly.age(), (unsigned int) 5);

}

TEST(AssemblyPoolTest, testAssembly) {

    AssemblyPool pool;

    {
        // simple case, a one-part message
        protocol::FragmentedNotificationPtr initialNotification(
                new protocol::FragmentedNotification);

        boost::uint32_t seqnum = 0;
        initialNotification->mutable_notification()->mutable_event_id()->set_sequence_number(
                seqnum);

        const string initialString = rsc::misc::randAlnumStr(10);
        initialNotification->mutable_notification()->set_data(initialString);

        initialNotification->set_num_data_parts(1);
        initialNotification->set_data_part(0);

        protocol::NotificationPtr result = pool.add(initialNotification);
        ASSERT_TRUE(result);
        EXPECT_EQ(initialString, result->data());

    }

    // complex case, a multi-part message
    {

        stringstream containedData;

        protocol::FragmentedNotificationPtr initialNotification(
                new protocol::FragmentedNotification);

        boost::uint32_t seqnum = 0;
        initialNotification->mutable_notification()->mutable_event_id()->set_sequence_number(
                seqnum);

        const string initialString = rsc::misc::randAlnumStr(10);
        initialNotification->mutable_notification()->set_data(initialString);
        containedData << initialString;

        const unsigned int dataParts = 5;
        initialNotification->set_num_data_parts(dataParts);
        initialNotification->set_data_part(0);
        EXPECT_FALSE(pool.add(initialNotification));

        for (unsigned int i = 1; i < dataParts; ++i) {

            protocol::FragmentedNotificationPtr newNotification(
                    new protocol::FragmentedNotification);

            const string newString = rsc::misc::randAlnumStr(30);
            newNotification->mutable_notification()->set_data(newString);
            containedData << newString;

            newNotification->mutable_notification()->mutable_event_id()->set_sequence_number(
                    seqnum);
            newNotification->set_num_data_parts(dataParts);
            newNotification->set_data_part(i);

            protocol::NotificationPtr result = pool.add(newNotification);

            if (i == (dataParts - 1)) {
                EXPECT_TRUE(result);
                EXPECT_EQ(containedData.str(), result->data());
            } else {
                EXPECT_FALSE(result);
            }

        }

    }

}

TEST(AssemblyPoolTest, testPruningDefaultOff) {

    AssemblyPool pool(1, 500);

    boost::uint32_t seqnum = 0;
    const unsigned int dataParts = 2;

    // add an initial multi-part message
    {
        protocol::FragmentedNotificationPtr initialNotification(
                new protocol::FragmentedNotification);

        initialNotification->mutable_notification()->mutable_event_id()->set_sequence_number(
                seqnum);

        const string initialString = rsc::misc::randAlnumStr(10);
        initialNotification->mutable_notification()->set_data(initialString);

        initialNotification->set_num_data_parts(dataParts);
        initialNotification->set_data_part(0);

        EXPECT_FALSE(pool.add(initialNotification));
    }

    // wait longer than the pruning time
    boost::this_thread::sleep(boost::posix_time::millisec(2500));

    // add the missing message fragment and expect a result
    {
        protocol::FragmentedNotificationPtr newNotification(
                new protocol::FragmentedNotification);

        const string newString = rsc::misc::randAlnumStr(30);
        newNotification->mutable_notification()->set_data(newString);

        newNotification->mutable_notification()->mutable_event_id()->set_sequence_number(
                seqnum);
        newNotification->set_num_data_parts(dataParts);
        newNotification->set_data_part(1);

        protocol::NotificationPtr result = pool.add(newNotification);
        EXPECT_TRUE(result);

    }

}

TEST(AssemblyPoolTest, testPruning) {

    AssemblyPool pool(1, 500);
    pool.setPruning(true);

    const unsigned int dataParts = 2;

    {
        boost::uint32_t seqnum = 0;
        // add an initial multi-part message
        {
            protocol::FragmentedNotificationPtr initialNotification(
                    new protocol::FragmentedNotification);

            initialNotification->mutable_notification()->mutable_event_id()->set_sequence_number(
                    seqnum);

            const string initialString = rsc::misc::randAlnumStr(10);
            initialNotification->mutable_notification()->set_data(
                    initialString);

            initialNotification->set_num_data_parts(dataParts);
            initialNotification->set_data_part(0);

            EXPECT_FALSE(pool.add(initialNotification));
        }

        // wait longer than the pruning time
        boost::this_thread::sleep(boost::posix_time::millisec(2500));

        // add the missing message fragment and expect no result
        {
            protocol::FragmentedNotificationPtr newNotification(
                    new protocol::FragmentedNotification);

            const string newString = rsc::misc::randAlnumStr(30);
            newNotification->mutable_notification()->set_data(newString);

            newNotification->mutable_notification()->mutable_event_id()->set_sequence_number(
                    seqnum);
            newNotification->set_num_data_parts(dataParts);
            newNotification->set_data_part(1);

            protocol::NotificationPtr result = pool.add(newNotification);

            EXPECT_FALSE(result);

        }
    }

    // now disable pruning
    pool.setPruning(false);

    // and check again that pruning is really of
    {
        boost::uint32_t seqnum = 1;
        // add an initial multi-part message
        {
            protocol::FragmentedNotificationPtr initialNotification(
                    new protocol::FragmentedNotification);

            initialNotification->mutable_notification()->mutable_event_id()->set_sequence_number(
                    seqnum);

            const string initialString = rsc::misc::randAlnumStr(10);
            initialNotification->mutable_notification()->set_data(
                    initialString);

            initialNotification->set_num_data_parts(dataParts);
            initialNotification->set_data_part(0);

            EXPECT_FALSE(pool.add(initialNotification));
        }

        // wait longer than the pruning time
        boost::this_thread::sleep(boost::posix_time::millisec(2500));

        // add the missing message fragment and expect a result
        {
            protocol::FragmentedNotificationPtr newNotification(
                    new protocol::FragmentedNotification);

            const string newString = rsc::misc::randAlnumStr(30);
            newNotification->mutable_notification()->set_data(newString);

            newNotification->mutable_notification()->mutable_event_id()->set_sequence_number(
                    seqnum);
            newNotification->set_num_data_parts(dataParts);
            newNotification->set_data_part(1);

            protocol::NotificationPtr result = pool.add(newNotification);

            EXPECT_TRUE(result);

        }
    }

}
