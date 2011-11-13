/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
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

#include "../../protocol/Notification.h"

#include "BusConnection.h"
#include "ConnectorBase.h"
#include "InPushConnector.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {
namespace socket {

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
class RSB_EXPORT Bus: public eventprocessing::Handler,
                      public boost::enable_shared_from_this<Bus> {
friend class BusConnection;
public:
    Bus(boost::asio::io_service& service);
    virtual ~Bus();

    void addSink(InPushConnectorPtr sink);
    void removeSink(InPushConnector* sink);

    void addConnector(/*ConnectorBasePtr*/ ConnectorBase* connector);
    void removeConnector(/*ConnectorBasePtr*/ConnectorBase* connector);

    /**
     * Adds @a connection to the list of connections of the bus. @a
     * connection should start receiving events, only after being
     * added to the bus.
     *
     * @param connection The connection that should be added.
     */
    void addConnection(BusConnectionPtr connection);

    /**
     * Removes @a connection from the list of connections of this
     * bus. @a connection is not closed or otherwise modified.
     *
     * @param connection The connection that should be removed.
     */
    void removeConnection(BusConnectionPtr connection);

    void handle(EventPtr event);
protected:
    virtual void handleIncoming(EventPtr event);

    virtual void suicide();
private:
    typedef std::list<BusConnectionPtr>                  ConnectionList;

    typedef std::list</*ConnectorBasePtr*/ConnectorBase*> ConnectorList;

    typedef std::list<boost::weak_ptr<InPushConnector> > SinkList;
    typedef std::map<Scope, SinkList>                    SinkMap;

    rsc::logging::LoggerPtr  logger;

    boost::asio::io_service& service;

    ConnectionList           connections;
    boost::recursive_mutex   connectionLock;

    ConnectorList            connectors;
    SinkMap                  sinks;
    boost::recursive_mutex   connectorLock;

    void printContents(std::ostream& stream) const;
};

}
}
}
