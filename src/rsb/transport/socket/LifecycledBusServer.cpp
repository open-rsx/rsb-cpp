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

#include "LifecycledBusServer.h"

using namespace std;

using namespace rsc::logging;

namespace rsb {
namespace transport {
namespace socket {

LifecycledBusServer::LifecycledBusServer(BusServerPtr server) :
        Bus(AsioServiceContextPtr()), BusServer(AsioServiceContextPtr(), false), logger(
                Logger::getLogger("rsb.transport.socket.LifecycledBusServer")), server(
                server) {
}

LifecycledBusServer::~LifecycledBusServer() {
    RSCDEBUG(logger, "Destructing");
    // This is the important aspect. We break the dependency cycle between the
    // asio service and BusServerImpl by explicitly disabling the service so
    // that asio releases the handle to BusServerImpl.
    this->server->deactivate();
}

void LifecycledBusServer::addSink(InConnectorPtr sink) {
    this->server->addSink(sink);
}

void LifecycledBusServer::removeSink(InConnector* sink) {
    this->server->removeSink(sink);
}

void LifecycledBusServer::addConnection(BusConnectionPtr connection) {
    this->server->addConnection(connection);
}

void LifecycledBusServer::removeConnection(BusConnectionPtr connection) {
    this->server->removeConnection(connection);
}

bool LifecycledBusServer::isTcpnodelay() const {
    return this->server->isTcpnodelay();
}

void LifecycledBusServer::handle(EventPtr event) {
    this->server->handle(event);
}

AsioServiceContextPtr LifecycledBusServer::getService() const {
    return server->getService();
}

void LifecycledBusServer::activate() {
    RSCDEBUG(logger, "Activating");
    this->server->activate();
}

void LifecycledBusServer::deactivate() {
    RSCDEBUG(logger, "Deactivating");
    this->server->deactivate();
}

void LifecycledBusServer::handleIncoming(EventPtr event,
        BusConnectionPtr connection) {
    this->server->handleIncoming(event, connection);
}

const string LifecycledBusServer::getTransportURL() const {
    return this->server->getTransportURL();
}

void LifecycledBusServer::printContents(std::ostream& stream) const {
    stream << this->server;
}

}
}
}
