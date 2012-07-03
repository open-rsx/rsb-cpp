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

#include <stdexcept>

#include <boost/thread.hpp>

#include "inprocess/InPullConnector.h"
#include "inprocess/InPushConnector.h"
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

#ifdef RSB_WITH_SPREAD_TRANSPORT

    // Verify that the version of the library that we linked against
    // is compatible with the version of the headers we compiled
    // against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

#endif

    // In-direction, push-style connectors
    {
        InPushFactory& factory = InPushFactory::getInstance();
        factory.registerConnector("inprocess",
                                  &inprocess::InPushConnector::create,
                                  "inprocess",
                                  false);

#ifdef RSB_WITH_SPREAD_TRANSPORT
        {
            set<string> options;
            options.insert("host");
            options.insert("port");

            factory.registerConnector("spread",
                                      &spread::InPushConnector::create,
                                      "spread",
                                      true,
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
                                      true,
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
                                      true,
                                      options);
        }
#endif

    }

    // Out-direction connectors
    {
        OutFactory& factory = OutFactory::getInstance();
        factory.registerConnector("inprocess",
                                  &inprocess::OutConnector::create,
                                  "inprocess",
                                  false);

#ifdef RSB_WITH_SPREAD_TRANSPORT
        {
            set<string> options;
            options.insert("host");
            options.insert("port");
            options.insert("maxfragmentsize");

            factory.registerConnector("spread",
                                      &spread::OutConnector::create,
                                      "spread",
                                      true,
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
                                      true,
                                      options);
        }
#endif

    }

}

bool isRemote(const string& transportName) {
    bool remote;
    bool validResult = false;
    try {
        InPullFactory& factory = InPullFactory::getInstance();
        InPullFactory::ConnectorInfo info
            = factory.getConnectorInfo(transportName);
        remote = info.isRemote();
        validResult = true;
    } catch (const rsc::runtime::NoSuchObject&) {
    }
    try {
        InPushFactory& factory = InPushFactory::getInstance();
        InPushFactory::ConnectorInfo info
            = factory.getConnectorInfo(transportName);
        if (validResult && (remote != info.isRemote())) {
            throw std::logic_error("connectors of one transport disagree about remoteness.");
        }
        remote = info.isRemote();
        validResult = true;
    } catch (const rsc::runtime::NoSuchObject&) {
    }
    try {
        OutFactory& factory = OutFactory::getInstance();
        OutFactory::ConnectorInfo info
            = factory.getConnectorInfo(transportName);
        if (validResult && (remote != info.isRemote())) {
            throw std::logic_error("connectors of one transport disagree about remoteness.");
        }
        remote = info.isRemote();
        validResult = true;
    } catch (const rsc::runtime::NoSuchObject&) {
    }
    if (!validResult) {
        throw rsc::runtime::NoSuchObject(transportName);
    }
    return remote;
}

}
}
