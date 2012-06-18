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

#include "Factory.h"

#include <boost/thread.hpp>

#include "inprocess/InConnector.h"
#include "inprocess/OutConnector.h"

#ifdef RSB_WITH_SPREAD_TRANSPORT
#include "spread/InPushConnector.h"
#include "spread/InPullConnector.h"
#include "spread/OutConnector.h"
#endif

#ifdef RSB_WITH_SOCKET_TRANSPORT
#include "socket/InPushConnector.h"
#include "socket/OutConnector.h"
#endif

#include "transports.h"

using namespace std;

namespace rsb {
namespace transport {

static bool registered = false;
static boost::mutex registrationMutex;

void registerDefaultTransports() {
    boost::mutex::scoped_lock lock(registrationMutex);

    if (registered) {
        return;
    }
    registered = true;

    // In-direction, push-style connectors
    {
        InPushFactory& factory = InPushFactory::getInstance();
        factory.registerConnector("inprocess",
                                  &inprocess::InConnector::create,
                                  "inprocess");

#ifdef RSB_WITH_SPREAD_TRANSPORT
        {
            set<string> options;
            options.insert("host");
            options.insert("port");

            factory.registerConnector("spread",
                                      &spread::InPushConnector::create,
                                      "spread",
                                      options);
        }
#endif

#ifdef RSB_WITH_SOCKET_TRANSPORT
        {
            set<string> options;
            options.insert("host");
            options.insert("port");
            options.insert("server");
            options.insert("tcpnodelay");

            factory.registerConnector("socket",
                                      &socket::InPushConnector::create,
                                      "socket",
                                      options);
        }
#endif

    }

    // In-direction, pull-style connectors
    {
        InPullFactory& factory = InPullFactory::getInstance();

#ifdef RSB_WITH_SPREAD_TRANSPORT
        {
            set<string> options;
            options.insert("host");
            options.insert("port");

            factory.registerConnector("spread",
                                      &spread::InPullConnector::create,
                                      "spread",
                                      options);
        }
#endif

    }

    // Out-direction connectors
    {
        OutFactory& factory = OutFactory::getInstance();
        factory.registerConnector("inprocess",
                                  &inprocess::OutConnector::create,
                                  "inprocess");

#ifdef RSB_WITH_SPREAD_TRANSPORT
        {
            set<string> options;
            options.insert("host");
            options.insert("port");
            options.insert("maxfragmentsize");

            factory.registerConnector("spread",
                                      &spread::OutConnector::create,
                                      "spread",
                                      options);
        }
#endif

#ifdef RSB_WITH_SOCKET_TRANSPORT
        {
            set<string> options;
            options.insert("host");
            options.insert("port");
            options.insert("server");
            options.insert("tcpnodelay");

            factory.registerConnector("socket",
                                      &socket::OutConnector::create,
                                      "socket",
                                      options);
        }
#endif

    }

}

}
}
