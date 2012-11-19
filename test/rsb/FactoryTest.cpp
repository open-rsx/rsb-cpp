/* ============================================================
 *
 * This file is a part of RSB project
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

#include <stdexcept>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/ParticipantConfig.h"
#include "rsb/Factory.h"
#include "rsb/LocalService.h"

using namespace std;
using namespace testing;
using namespace rsb;

TEST(FactoryTest, testDefaultParticipantConfig)
{

    Factory& f = getFactory();

    ParticipantConfig config;
    config.addTransport(ParticipantConfig::Transport("foo"));
    config.addTransport(ParticipantConfig::Transport("bar"));

    f.setDefaultParticipantConfig(config);
    EXPECT_EQ(size_t(2), f.getDefaultParticipantConfig().getTransports().size());

}

TEST(FactoryTest, testCreateService)
{

    Factory& f = getFactory();

    Scope scopeA("/a");
    Scope scopeB("/b/with/sub");

    ServicePtr sA = f.createService(scopeA);
    ServicePtr sB = f.createService(scopeB);

    EXPECT_TRUE(sA);
    EXPECT_TRUE(sB);

    EXPECT_EQ(scopeA, *sA->getScope());
    EXPECT_EQ(scopeB, *sB->getScope());

    EXPECT_TRUE(boost::dynamic_pointer_cast<LocalService>(sA));
    EXPECT_TRUE(boost::dynamic_pointer_cast<LocalService>(sB));

}

TEST(FactoryTest, testLegacyInstantiation) {
    EXPECT_EQ((void*) &getFactory(), (void*) &(Factory::getInstance()));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
