/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2012 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "rsb/transport/inprocess/InConnector.h"
#include "rsb/transport/inprocess/OutConnector.h"

#include "testconfig.h"
#include "../ConnectorTest.h"

using namespace std;
using namespace testing;

using namespace rsb::transport;
using namespace rsb::inprocess;

static int dummy = pullInConnectorTest();

InPullConnectorPtr createInProcessInPullConnector() {
    return InPullConnectorPtr((rsb::transport::InPullConnector*) 0
                              /*new rsb::inprocess::InPullConnector()*/);
}

InPushConnectorPtr createInProcessInPushConnector() {
    return InPushConnectorPtr(new rsb::inprocess::InConnector());
}

OutConnectorPtr createInProcessOutConnector() {
    return OutConnectorPtr(new rsb::inprocess::OutConnector());
}

const ConnectorTestSetup inProcessSetup(createInProcessInPullConnector,
                                        createInProcessInPushConnector,
                                        createInProcessOutConnector);

INSTANTIATE_TEST_CASE_P(InProcessConnector,
        ConnectorTest,
        ::testing::Values(inProcessSetup))
;
