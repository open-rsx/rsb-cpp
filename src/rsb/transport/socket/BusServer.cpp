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

#include "BusServer.h"

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

BusServer::BusServer(boost::uint16_t port,
                     bool            tcpnodelay,
                     io_service&     service)
    : Bus(service, tcpnodelay),
      logger(Logger::getLogger("rsb.transport.socket.BusServer")),
      acceptor(service, tcp::endpoint(tcp::v4(), port)),
      service(service),
      shutdown(false) {
    acceptOne();
}


BusServer::~BusServer() {
    this->shutdown = true;
    this->acceptor.cancel();
    boost::this_thread::sleep(boost::posix_time::seconds(1)); /** TODO(jmoringe, 2012-03-02): hack */
}

void BusServer::acceptOne() {
    SocketPtr socket(new tcp::socket(this->service));

    RSCINFO(logger, "Listening on " << this->acceptor.local_endpoint());
    acceptor.async_accept(*socket,
                          boost::bind(&BusServer::handleAccept, this, socket,
                                      boost::asio::placeholders::error));
}

void BusServer::handleAccept(SocketPtr                        socket,
                             const boost::system::error_code& error) {
    if (!error) {
        RSCINFO(logger, "Got connection from " << socket->remote_endpoint());

        BusConnectionPtr connection(new BusConnection(shared_from_this(), socket, false, isTcpnodelay()));
        addConnection(connection);
        connection->startReceiving();
    } else if (!this->shutdown){
        RSCWARN(logger, "Accept failure, trying to continue");
    }

    // Maybe continue accepting connections.
    if (!this->shutdown) {
        acceptOne();
    }
}

void BusServer::handleIncoming(EventPtr         event,
                               BusConnectionPtr connection) {
    Bus::handleIncoming(event, connection);

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

void BusServer::suicide() {
    Factory::getInstance().removeBusServer(shared_from_this());
}

}
}
}
