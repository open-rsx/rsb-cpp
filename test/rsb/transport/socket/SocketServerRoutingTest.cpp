/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2012 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <rsc/misc/IllegalStateException.h>
#include <rsc/misc/langutils.h>

#include "rsb/Event.h"
#include "rsb/Factory.h"
#include "rsb/converter/Repository.h"
#include "rsb/transport/socket/InPullConnector.h"
#include "rsb/transport/socket/InPushConnector.h"
#include "rsb/transport/socket/OutConnector.h"

#include "testconfig.h"

using namespace std;
using namespace rsb;
using namespace rsb::transport::socket;
using namespace rsb::converter;
using namespace testing;

TEST(SocketServerRoutingTest, testEventRouting) {

    ::rsb::getFactory();

    rsb::transport::OutConnectorPtr sender(
            new rsb::transport::socket::OutConnector(
                    getDefaultFactory(),
                    converterRepository<string>()->getConvertersForSerialization(),
                    "localhost", SOCKET_PORT, rsb::transport::socket::SERVER_YES,
                    true));

    rsb::transport::InPullConnectorPtr clientReceiver(
            new rsb::transport::socket::InPullConnector(
                    getDefaultFactory(),
                    converterRepository<string>()->getConvertersForDeserialization(),
                    "localhost", SOCKET_PORT, rsb::transport::socket::SERVER_NO,
                    true));

    rsb::transport::InPushConnectorPtr serverPushReceiver(
            new rsb::transport::socket::InPushConnector(
                    getDefaultFactory(),
                    converterRepository<string>()->getConvertersForDeserialization(),
                    "localhost", SOCKET_PORT, rsb::transport::socket::SERVER_YES,
                    true));

    rsb::transport::InPullConnectorPtr serverPullReceiver(
            new rsb::transport::socket::InPullConnector(
                    getDefaultFactory(),
                    converterRepository<string>()->getConvertersForDeserialization(),
                    "localhost", SOCKET_PORT,
                    rsb::transport::socket::SERVER_YES, true));

    Scope scope("/test/scope");

    sender->activate();
    clientReceiver->activate();
    serverPullReceiver->activate();
    serverPushReceiver->activate();

    EventPtr event(new Event);
    event->setEventId(rsc::misc::UUID(), 42);
    event->setType(rsc::runtime::typeName<boost::uint64_t>());
    event->setData(VoidPtr(new boost::uint64_t(123123)));
    event->setScope(scope);

    sender->handle(event);

    clientReceiver->raiseEvent(true);
    serverPullReceiver->raiseEvent(true);

    clientReceiver->deactivate();
    serverPullReceiver->deactivate();
    serverPushReceiver->deactivate();
    sender->deactivate();

}
