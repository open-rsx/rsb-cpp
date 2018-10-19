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

#include "BusServerImpl.h"

#include <list>

#include <boost/bind.hpp>

#include <boost/thread/thread_time.hpp>

#include "../../MetaData.h"
#include "Factory.h"

using namespace std;

using namespace boost::asio;
using boost::asio::ip::tcp;

using namespace rsc::logging;

namespace rsb {
namespace transport {
namespace socket {

BusServerImpl::BusServerImpl(AsioServiceContextPtr asioService,
                             boost::uint16_t       port,
                             bool                  tcpnodelay,
                             bool                  waitForClientDisconnects)
    : BusImpl(asioService, tcpnodelay),
      logger(Logger::getLogger("rsb.transport.socket.BusServerImpl")),
      acceptor(*this->getService()->getService(), tcp::endpoint(tcp::v4(), port)),
      active(false), shutdown(false),
      waitForClientDisconnects(waitForClientDisconnects) {
}


BusServerImpl::~BusServerImpl() {
    RSCDEBUG(logger, "Destructing");
    if (this->active) {
        deactivate();
    }
}

void BusServerImpl::activate() {
    RSCDEBUG(logger, "Activating");
    acceptOne(shared_from_this());

    this->active = true;
}

void BusServerImpl::deactivate() {
    RSCDEBUG(logger, "Deactivating");
    // Initiate shutdown sequence, cancel acceptor and wait until the
    // asynchronous callbacks signal completion of the shutdown
    // sequence (see handleAccept()).
    this->shutdown = true;
    this->acceptor.cancel();
    while (this->shutdown) {
        // ugly active shutdown
    }

    {
        boost::unique_lock<boost::recursive_mutex> lock(getConnectionLock());
        if (!getConnections().empty()) {
            RSCWARN(logger, "Active client connections exist");
        }
        if (this->waitForClientDisconnects) {
            RSCDEBUG(logger, "Waiting for client connections to terminate");
            while (!getConnections().empty()) {
                this->shutdownCondition.wait(lock);
            }
        } else {
            RSCWARN(logger, "Not waiting for client connections as requested");
        }
    }

    this->active =  false;
}

void BusServerImpl::removeConnection(BusConnectionPtr connection) {
    boost::recursive_mutex::scoped_lock lock(getConnectionLock());
    BusImpl::removeConnection(connection);
    this->shutdownCondition.notify_one();
}

void BusServerImpl::acceptOne(boost::shared_ptr<BusServerImpl> ref) {
    SocketPtr socket(new tcp::socket(*this->getService()->getService()));

    RSCINFO(logger, "Listening on " << this->acceptor.local_endpoint());
    acceptor.async_accept(*socket,
                          boost::bind(&BusServerImpl::handleAccept, this, ref, socket,
                                      boost::asio::placeholders::error));
}

void BusServerImpl::handleAccept(boost::shared_ptr<BusServerImpl> ref,
                                 SocketPtr                        socket,
                                 const boost::system::error_code& error) {
    if (!error) {
        RSCINFO(logger, "Got connection from " << socket->remote_endpoint());

        BusConnectionPtr connection(new BusConnection(ref, socket, false, isTcpnodelay()));
        addConnection(connection);
        connection->startReceiving();
    } else if (!this->shutdown){
        RSCWARN(logger, "Accept failure, trying to continue");
    }

    // Maybe continue accepting connections. If not, a shutdown has
    // been requested from another thread. In that case, we reset
    // this->shutdown to false to indicate that the shutdown sequence
    // is complete. The other thread can just busy-wait until
    // this->shutdown == false.
    if (!this->shutdown) {
        acceptOne(ref);
    } else {
        this->shutdown = false;
    }
}

void BusServerImpl::handleIncoming(EventPtr         event,
                                   BusConnectionPtr connection) {
    BusImpl::handleIncoming(event, connection);

    RSCDEBUG(logger, "Delivering received event to connections " << event);
    {
        boost::recursive_mutex::scoped_lock lock(getConnectionLock());

        ConnectionList connections = getConnections();
        list<BusConnectionPtr> failing;
        for (ConnectionList::iterator it = connections.begin();
             it != connections.end(); ++it) {
            if (*it != connection) {
                RSCDEBUG(logger, "Delivering to connection " << *it);
                try {
                    (*it)->sendEvent(event, event->getMetaData().getUserInfo("rsb.wire-schema"));
                } catch (const std::exception& e) {
                    RSCWARN(logger, "Send failure (" << e.what() << "); will close connection later");
                    // We record failing connections instead of
                    // closing them immediately to avoid invalidating
                    // the iterator.
                    failing.push_back(*it);
                }
            }
        }

        // This should remove all references to the connection
        // objects.
        for (list<BusConnectionPtr>::const_iterator it = failing.begin();
        it != failing.end(); ++it) {
            removeConnection(*it);
        }
    }
}

const std::string BusServerImpl::getTransportURL() const {
    return boost::str(boost::format("socket://%1%:%2%")
                      % this->acceptor.local_endpoint().address().to_string()
                      % this->acceptor.local_endpoint().port());
}


}
}
}
