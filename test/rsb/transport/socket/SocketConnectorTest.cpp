/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011-2017 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "rsb/transport/socket/InPullConnector.h"
#include "rsb/transport/socket/InPushConnector.h"
#include "rsb/transport/socket/OutConnector.h"

#include "testconfig.h"
#include "../ConnectorTest.h"

using namespace std;
using namespace testing;
using namespace rsb::converter;

static int dummy
#if defined(__GNUC__)
__attribute__((used))
#endif
= pullInConnectorTest();

rsb::transport::InPullConnectorPtr createSocketInPullConnector() {
    return rsb::transport::InPullConnectorPtr(
            new rsb::transport::socket::InPullConnector(
                    rsb::transport::socket::getDefaultFactory(),
                    converterRepository<string>()->getConvertersForDeserialization(),
                    "localhost", SOCKET_PORT,
                    rsb::transport::socket::SERVER_AUTO, true));
}

rsb::transport::InPushConnectorPtr createSocketInPushConnector() {
    return rsb::transport::InPushConnectorPtr(
            new rsb::transport::socket::InPushConnector(
                    rsb::transport::socket::getDefaultFactory(),
                    converterRepository<string>()->getConvertersForDeserialization(),
                    "localhost", SOCKET_PORT,
                    rsb::transport::socket::SERVER_AUTO, true));
}

rsb::transport::OutConnectorPtr createSocketOutConnector() {
    return rsb::transport::OutConnectorPtr(
            new rsb::transport::socket::OutConnector(
                    rsb::transport::socket::getDefaultFactory(),
                    converterRepository<string>()->getConvertersForSerialization(),
                    "localhost", SOCKET_PORT,
                    rsb::transport::socket::SERVER_AUTO, true));
}

const ConnectorTestSetup socketSetup(createSocketInPullConnector,
        createSocketInPushConnector, createSocketOutConnector);

INSTANTIATE_TEST_CASE_P(SocketConnector, ConnectorTest,
        ::testing::Values(socketSetup));
