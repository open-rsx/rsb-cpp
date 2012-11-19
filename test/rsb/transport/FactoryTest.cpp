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

#include <rsb/transport/Factory.h>

#include <gtest/gtest.h>

using namespace std;

using namespace rsc::runtime;

using namespace rsb;
using namespace rsb::transport;

class TestConnector: public InPullConnector {
public:
    static InPullConnector* create(const Properties&) {
        return new TestConnector();
    }

    virtual ~TestConnector() {
    }

    void activate() {
    }

    void deactivate() {
    }

    void setQualityOfServiceSpecs(const QualityOfServiceSpec&) {}

    void setScope(const Scope&) {}

    string getClassName() const {
        return "TestConnector";
    }

    EventPtr raiseEvent(bool /*block*/) {
        return EventPtr();
    }

};

TEST(ConnectorFactoryTest, testRegister)
{
    ConnectorFactory<InPullConnector>& factory = getInPullFactory();
    factory.registerConnector("bla", &TestConnector::create, "bla");
    factory.getConnectorInfo("bla");
}

TEST(ConnectorFactoryTest, testLegacyInstantiation) {
    EXPECT_EQ((void*) &getInPullFactory(), (void*) &(InPullFactory::getInstance()));
    EXPECT_EQ((void*) &getInPushFactory(), (void*) &(InPushFactory::getInstance()));
    EXPECT_EQ((void*) &getOutFactory(), (void*) &(OutFactory::getInstance()));
}
