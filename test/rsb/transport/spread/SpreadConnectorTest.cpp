/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include "rsb/converter/Repository.h"
#include "rsb/transport/spread/InConnector.h"
#include "rsb/transport/spread/OutConnector.h"

#include "testconfig.h"
#include "../ConnectorTest.h"

using namespace std;
using namespace rsb::spread;
using namespace rsb::transport;
using namespace rsb::converter;
using namespace testing;

static int dummy = pullInConnectorTest();

InPushConnectorPtr createSpreadInConnector() {
    return InPushConnectorPtr(new rsb::spread::InConnector(converterRepository<string>()->getConvertersForDeserialization(),
						       defaultHost(),
						       SPREAD_PORT));
}

OutConnectorPtr createSpreadOutConnector() {
    return OutConnectorPtr(new rsb::spread::OutConnector(converterRepository<string>()->getConvertersForSerialization(),
							 defaultHost(),
							 SPREAD_PORT));
}

const ConnectorTestSetup spreadSetup(createSpreadInConnector,
        createSpreadOutConnector);

INSTANTIATE_TEST_CASE_P(SpreadConnector,
        ConnectorTest,
        ::testing::Values(spreadSetup))
;
