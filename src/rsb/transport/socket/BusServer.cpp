/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <boost/bind.hpp>

#include "../../MetaData.h"
#include "Factory.h"

using namespace boost;

using namespace boost::asio;
using boost::asio::ip::tcp;

using namespace rsc::logging;

namespace rsb {
namespace transport {
namespace socket {

BusServer::BusServer(uint16_t    port,
                     bool        tcpnodelay,
                     io_service& service)
    : Bus(service, tcpnodelay),
      logger(Logger::getLogger("rsb.transport.socket.BusServer")),
      acceptor(service, tcp::endpoint(tcp::v4(), port)),
      service(service) {
    acceptOne();
}

void BusServer::acceptOne() {
    SocketPtr socket(new tcp::socket(this->service));

    RSCINFO(logger, "Listening on " << this->acceptor.local_endpoint());
    acceptor.async_accept(*socket,
                          bind(&BusServer::handleAccept, this, socket,
                               placeholders::error));
}

void BusServer::handleAccept(SocketPtr                 socket,
                             const boost::system::error_code& error) {
    if (!error) {
        //
        RSCINFO(logger, "Got connection from " << socket->remote_endpoint());

        BusConnectionPtr connection(new BusConnection(shared_from_this(), socket, false, isTcpnodelay()));
        addConnection(connection);
        connection->startReceiving();
    } else {
        RSCWARN(logger, "Accept failure, trying to continue");
    }

    // Continue accepting connections.
    acceptOne();
}

void BusServer::handleIncoming(EventPtr         event,
                               BusConnectionPtr connection) {
    Bus::handleIncoming(event, connection);

    RSCDEBUG(logger, "Delivering received event to connections " << event);
    {
        recursive_mutex::scoped_lock lock(getConnectionLock());

        ConnectionList connections = getConnections();
        for (ConnectionList::iterator it = connections.begin();
             it != connections.end(); ++it) {
            if (*it != connection) {
                RSCDEBUG(logger, "Delivering to connection " << *it);
                (*it)->sendEvent(event, event->getMetaData().getUserInfo("rsb.wire-schema"));
            }
        }
    }
}

void BusServer::suicide() {
    Factory::getInstance().removeBusServer(shared_from_this());
}

}
}
}
