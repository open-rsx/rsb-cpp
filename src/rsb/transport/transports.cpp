/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011-2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "transports.h"

#include "Factory.h"

#include <stdexcept>

#include <boost/thread.hpp>

#include "inprocess/InConnector.h"
#include "inprocess/OutConnector.h"

#ifdef RSB_WITH_SOCKET_TRANSPORT
#include "socket/InConnector.h"
#include "socket/OutConnector.h"
#endif

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

    // In-direction connectors
    {
        InFactory& factory = getInFactory();
        factory.registerConnector("inprocess",
                                  &inprocess::InConnector::create,
                                  "inprocess",
                                  false);

#ifdef RSB_WITH_SOCKET_TRANSPORT
        {
            set<string> options;
            options.insert("host");
            options.insert("port");
            options.insert("server");
            options.insert("tcpnodelay");
            options.insert("wait");

            factory.registerConnector("socket",
                                      &socket::InConnector::create,
                                      "socket",
                                      true,
                                      options);
        }
#endif

    }

    // Out-direction connectors
    {
        OutFactory& factory = getOutFactory();
        factory.registerConnector("inprocess",
                                  &inprocess::OutConnector::create,
                                  "inprocess",
                                  false);

#ifdef RSB_WITH_SOCKET_TRANSPORT
        {
            set<string> options;
            options.insert("host");
            options.insert("port");
            options.insert("server");
            options.insert("tcpnodelay");
            options.insert("wait");

            factory.registerConnector("socket",
                                      &socket::OutConnector::create,
                                      "socket",
                                      true,
                                      options);
        }
#endif

    }

}

set<string> getAvailableTransports(unsigned int requiredDirections) {
    struct intersection {
        set<string> operator()(const set<string>& left,
                               const set<string>& right) const {
            set<string> result;
            set_intersection(left.begin(), left.end(), right.begin(), right.end(),
                             inserter(result, result.begin()));
            return  result;
        }
    };

    if (requiredDirections == 0) {
        throw invalid_argument("At least one required direction has to be specified.");
    }

    set<string> result;
    set<string> inTransports;
    {
        set<InFactory::ConnectorInfo> infos = getInFactory().getConnectorInfos();
        for (set<InFactory::ConnectorInfo>::const_iterator it
                 = infos.begin(); it != infos.end(); ++it) {
            inTransports.insert(it->getName());
            result.insert(it->getName());
        }
    }
    set<string> outTransports;
    {
        set<OutFactory::ConnectorInfo> infos = getOutFactory().getConnectorInfos();
        for (set<OutFactory::ConnectorInfo>::const_iterator it
                 = infos.begin(); it != infos.end(); ++it) {
            outTransports.insert(it->getName());
            result.insert(it->getName());
        }
    }

    if (requiredDirections & DIRECTION_IN) {
        result = intersection()(result, inTransports);
    }
    if (requiredDirections & DIRECTION_OUT) {
        result = intersection()(result, outTransports);
    }
    return result;
}


bool isAvailable(const string& transportName,
                 unsigned int  requiredDirections) {
    if (requiredDirections == 0) {
        throw invalid_argument("At least one required direction has to be specified.");
    }

    if (requiredDirections & DIRECTION_IN) {
        try {
            getInFactory().getConnectorInfo(transportName);
        } catch (const rsc::runtime::NoSuchObject&) {
            return false;
        }
    }
    if (requiredDirections & DIRECTION_OUT) {
        try {
            getOutFactory().getConnectorInfo(transportName);
        } catch (const rsc::runtime::NoSuchObject&) {
            return false;
        }
    }
    return true;
}

bool isRemote(const string& transportName) {
    bool remote = false;
    bool validResult = false;
    try {
        InFactory& factory = getInFactory();
        InFactory::ConnectorInfo info
            = factory.getConnectorInfo(transportName);
        remote = info.isRemote();
        validResult = true;
    } catch (const rsc::runtime::NoSuchObject&) {
    }
    try {
        OutFactory& factory = getOutFactory();
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
