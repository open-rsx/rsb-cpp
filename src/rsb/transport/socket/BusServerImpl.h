/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2012, 2015, 2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <boost/cstdint.hpp>

#include <boost/shared_ptr.hpp>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread/condition_variable.hpp>

#include <rsc/logging/Logger.h>

#include "BusImpl.h"
#include "BusServer.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {
namespace socket {

/**
 * Instances of this class provide access to a socket-based bus for
 * local and remote bus clients.
 *
 * Remote clients can connect to a server socket in order to send and
 * receive events through the resulting socket connection.
 *
 * Local clients (connectors) use the usual @ref Bus interface to
 * receive events submitted by remote clients and submit events which
 * will be distributed to remote clients by the @ref BusServerImpl.
 *
 * @author jmoringe
 */
class RSB_EXPORT BusServerImpl : public virtual BusImpl,
                                 public virtual BusServer,
                                 public boost::enable_shared_from_this<BusServerImpl> {
public:
    BusServerImpl(AsioServiceContextPtr    asioService,
                  boost::uint16_t          port,
                  bool                     tcpnodelay,
                  bool                     waitForClientDisconnects);

    virtual ~BusServerImpl();

    /**
     * Activate the object.
     *
     * @note This member function can only be called when a @ref
     *       boost::shared_ptr owning the object exists.
     */
    void activate();

    void deactivate();

    virtual void removeConnection(BusConnectionPtr connection);

    void handleIncoming(EventPtr         event,
                        BusConnectionPtr connection);

    virtual const std::string getTransportURL() const;
protected:
    typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;
private:

    rsc::logging::LoggerPtr         logger;

    boost::asio::ip::tcp::acceptor  acceptor;

    volatile bool                   active;
    volatile bool                   shutdown;

    bool                            waitForClientDisconnects;

    boost::condition_variable_any   shutdownCondition;

    // These two member functions have the additional ref parameter to
    // ensure that the BusServerImpl object cannot be destroyed while
    // callbacks are executed. This also means that
    // BusServerImpl::activate only when there is a shared_ptr owning the
    // BusServerImpl object. See BusServerImpl::activate().
    void acceptOne(boost::shared_ptr<BusServerImpl> ref);

    void handleAccept(boost::shared_ptr<BusServerImpl> ref,
                      SocketPtr                        socket,
                      const boost::system::error_code& error);

};

}
}
}
