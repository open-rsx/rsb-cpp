/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2014 Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "BusServer.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {
namespace socket {

/**
 * A facade around @ref BusServer instances to allow breaking dependency cycles.
 * This implementation specifically calls deactivate on the underlying server
 * instance in its destructor. This is known to break a dependency cycle in
 * @ref BusServerImpl which would otherwise prevent clean destruction through
 * shared_ptr instances.
 *
 * Apart from this behavior, all calls are passed to the underlying instance.
 *
 * @author jwienke
 */
class RSB_EXPORT LifecycledBusServer : public BusServer {
public:
    LifecycledBusServer(BusServerPtr server);

    virtual ~LifecycledBusServer();

    virtual void addSink(InConnectorPtr sink);
    virtual void removeSink(InConnector* sink);

    virtual void addConnection(BusConnectionPtr connection);
    virtual void removeConnection(BusConnectionPtr connection);

    virtual bool isTcpnodelay() const;

    virtual void handle(EventPtr event);

    virtual AsioServiceContextPtr getService() const;

    void activate();

    void deactivate();

    void handleIncoming(EventPtr         event,
                        BusConnectionPtr connection);

    virtual void printContents(std::ostream& stream) const;

private:
    rsc::logging::LoggerPtr         logger;
    BusServerPtr                    server;

};

}
}
}
