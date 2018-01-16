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

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

#include "../../Event.h"

#include "../../eventprocessing/Handler.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {
namespace socket {

class InConnector;
typedef boost::shared_ptr<InConnector> InConnectorPtr;

class BusConnection;
typedef boost::shared_ptr<BusConnection> BusConnectionPtr;

/**
 * Instances of this class provide access to a socket-based bus.
 *
 * It is transparent for clients (connectors) of this class whether is
 * accessed by running the bus server or by connecting to the bus
 * server as a client.
 *
 * In-direction connectors add themselves as event sinks using the
 * @ref addSink method.
 *
 * Out-direction connectors submit events to the bus using the @ref
 * handle method.
 *
 * @author jmoringe
 */
class RSB_EXPORT Bus: public eventprocessing::Handler {
friend class BusConnection;
public:
    virtual ~Bus();

    virtual void addSink(InConnectorPtr sink) = 0;
    virtual void removeSink(const InConnector* sink) = 0;

    /**
     * Adds @a connection to the list of connections of the bus. @a
     * connection should start receiving events, only after being
     * added to the bus.
     *
     * @param connection The connection that should be added.
     */
    virtual void addConnection(BusConnectionPtr connection) = 0;

    /**
     * Removes @a connection from the list of connections of this
     * bus. @a connection is not closed or otherwise modified.
     *
     * @param connection The connection that should be removed.
     */
    virtual void removeConnection(BusConnectionPtr connection) = 0;

    virtual bool isTcpnodelay() const = 0;

    virtual void handle(EventPtr event) = 0;

    virtual void handleIncoming(EventPtr         event,
                                BusConnectionPtr connection) = 0;

    virtual const std::string getTransportURL() const = 0;
};

typedef boost::shared_ptr<Bus> BusPtr;

}
}
}
