/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2012-2018 Jan Moringen <jmoringe@techfak.uni-bielfeld.de>
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

#include <boost/function.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/transport/InConnector.h"
#include "rsb/transport/OutConnector.h"

/**
 * Test configuration for different connector types.
 *
 * @author jwienke
 */
class ConnectorTestSetup {
public:
    ConnectorTestSetup(
            boost::function<rsb::transport::InConnectorPtr()> createInConnector,
            boost::function<rsb::transport::OutConnectorPtr()> createOutConnector) :
        createInConnector(createInConnector),
        createOutConnector(createOutConnector) {
    }
    boost::function<rsb::transport::InConnectorPtr()> createInConnector;
    boost::function<rsb::transport::OutConnectorPtr()> createOutConnector;
};

/**
 * Value-parametrized test class for different connector
 * implementations. For each implementation provide a @c
 * ConnectorTestSetup instance that specifies factory functions to use
 * for creating connectors.
 *
 * @author jwienke
 */
class ConnectorTest: public ::testing::TestWithParam<ConnectorTestSetup> {
public:
    virtual ~ConnectorTest() {
    }
};

int pullInConnectorTest();
