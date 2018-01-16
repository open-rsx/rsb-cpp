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
#include <map>
#include <list>

#include <boost/shared_ptr.hpp>

#include <boost/thread/recursive_mutex.hpp>

#include <rsc/logging/Logger.h>

#include "../../Event.h"
#include "../../Scope.h"

#include "../../eventprocessing/Handler.h"

#include "../AsioServiceContext.h"

#include "Bus.h"
#include "BusConnection.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {
namespace socket {

class InConnector;
typedef boost::shared_ptr<InConnector> InConnectorPtr;

class ConnectorBase;

/**
 * @author jmoringe
 */
class RSB_EXPORT BusImpl: public virtual Bus {
friend class BusConnection;
public:
    BusImpl(AsioServiceContextPtr asioService,
            bool                  tcpnodelay = false);
    virtual ~BusImpl();

    virtual void addSink(InConnectorPtr sink);
    virtual void removeSink(InConnector* sink);

    /**
     * Adds @a connection to the list of connections of the bus. @a
     * connection should start receiving events, only after being
     * added to the bus.
     *
     * @param connection The connection that should be added.
     */
    virtual void addConnection(BusConnectionPtr connection);

    /**
     * Removes @a connection from the list of connections of this
     * bus. @a connection is not closed or otherwise modified.
     *
     * @param connection The connection that should be removed.
     */
    virtual void removeConnection(BusConnectionPtr connection);

    virtual bool isTcpnodelay() const;

    virtual void handle(EventPtr event);

    virtual void handleIncoming(EventPtr         event,
                                BusConnectionPtr connection);

    virtual void printContents(std::ostream& stream) const;

    virtual const std::string getTransportURL() const;

protected:
    typedef std::list<BusConnectionPtr> ConnectionList;

    ConnectionList getConnections() const;
    boost::recursive_mutex& getConnectionLock();

    virtual AsioServiceContextPtr getService() const;
private:
    typedef std::list<boost::weak_ptr<InConnector> > SinkList;
    typedef std::map<Scope, SinkList>                SinkMap;

    rsc::logging::LoggerPtr  logger;

    // the asio service this bus operates on. Storing this pointer keeps the
    // service alive as long as this instance is alive
    AsioServiceContextPtr    asioService;

    ConnectionList           connections;
    boost::recursive_mutex   connectionLock;

    SinkMap                  sinks;
    boost::recursive_mutex   connectorLock;

    bool                     tcpnodelay;

};

}
}
}
