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

#include "Factory.h"

#include <boost/lexical_cast.hpp>

#include <boost/asio/ip/address.hpp>

#include <boost/format.hpp>

#include <rsc/runtime/ContainerIO.h>

#include "BusServerImpl.h"
#include "LifecycledBusServer.h"

using namespace std;

using namespace boost;

using namespace boost::asio;
using boost::asio::ip::tcp;

using namespace rsc::logging;

namespace rsb {
namespace transport {
namespace socket {

// Create and start an io_service. This service will be shared between
// all bus providers created by this factory.
Factory::Factory() :
        logger(Logger::getLogger("rsb.transport.socket.Factory")), asioService(
                new AsioServiceContext) {
    RSCDEBUG(logger, "Constructed and asio service created");
}

Factory::~Factory() {
    RSCDEBUG(logger, "Destructing");
}

template<class BusType>
boost::shared_ptr<BusType> Factory::searchInMap(const Endpoint& endpoint,
        bool tcpnodelay, map<Endpoint, boost::weak_ptr<BusType> >& map) {
    typename std::map<Endpoint, boost::weak_ptr<BusType> >::const_iterator it;
    if ((it = map.find(endpoint)) != map.end()) {
        boost::shared_ptr<BusType> result = it->second.lock();
        if (result) {
            checkOptions(result, tcpnodelay);
            RSCDEBUG(logger,
                    "Found existing bus " << result
                            << " without resolving");
            return result;
        } else {
            map.erase(endpoint);
        }
    }
    return boost::shared_ptr<BusType>();
}

BusPtr Factory::getBusClientFor(const string&  host,
                                uint16_t       port,
                                bool           tcpnodelay) {
    RSCDEBUG(logger, "Was asked for a bus client for " << host << ":" << port);

    // Try to find an entry for the exact specified endpoint. If this
    // yields a hit, there is no need to resolve the specified name.
    Endpoint endpoint(host, port);

    {
        BusPtr result = searchInMap(endpoint, tcpnodelay, busClients);
        if (result) {
            return result;
        }

        RSCDEBUG(logger, "Did not find bus client without resolving");
    }

    // We did not find an entry for the exact specified entry. We try
    // to resolve it to a working endpoint and use that one in the
    // lookup.
    // TODO(jmoringe): avoid this useless socket connection just for
    // the lookup
    RSCDEBUG(logger, "Resolving endpoint")
    tcp::resolver resolver(*this->asioService->getService());
    tcp::resolver::query query(host, lexical_cast<string>(port),
                               tcp::resolver::query::numeric_service);
    for (tcp::resolver::iterator endpointIterator = resolver.resolve(query);
         endpointIterator != tcp::resolver::iterator();
         ++endpointIterator) {
        endpoint = Endpoint(endpointIterator->host_name(), port);
        // When we have a working endpoint, repeat the lookup.
        BusPtr result = searchInMap(endpoint, tcpnodelay, busClients);
        if (result) {
            return result;
        }
    }

    // Try to open a socket for the resolved endpoint.
    SocketPtr socket;
    for (tcp::resolver::iterator endpointIterator = resolver.resolve(query);
         endpointIterator != tcp::resolver::iterator();
         ++endpointIterator) {
        endpoint = Endpoint(endpointIterator->host_name(), port);
        RSCDEBUG(logger, "Trying endpoint " << endpointIterator->endpoint());
        socket.reset(new tcp::socket(*this->asioService->getService()));
        boost::system::error_code error;
        socket->connect(endpointIterator->endpoint(), error);
        if (!error) {
            RSCDEBUG(logger, "Success");
            break;
        }
        RSCDEBUG(logger, "Failed: " << error.message());
        socket.reset();
    }
    if (!socket) {
        throw runtime_error(str(format("Could not connect to any of the endpoints to which %1%:%2% resolved.")
                                % host % port));
    }

    // Name resolution did not yield any endpoints, or none of the
    // worked. Create a new bus client.
    RSCDEBUG(logger, "Did not find bus client after resolving; creating a new one");

    BusPtr result(new Bus(this->asioService, tcpnodelay));
    this->busClients[endpoint] = result;

    BusConnectionPtr connection(new BusConnection(result, socket, true, tcpnodelay));
    result->addConnection(connection);
    connection->startReceiving();

    RSCDEBUG(logger, "Created new bus client " << result);

    return result;
}

BusServerPtr Factory::getBusServerFor(const string&  host,
                                      uint16_t       port,
                                      bool           tcpnodelay) {
    RSCDEBUG(logger, "Was asked for a bus server for " << host << ":" << port);

    // Try to find an existing entry for the specified endpoint.
    Endpoint endpoint(host, port);

    BusServerPtr result = searchInMap(endpoint, tcpnodelay, busServers);
    if (result) {
        return result;
    }

    // If there is no entry, create a new bus server and put it into
    // the map.
    RSCDEBUG(logger, "Did not find bus server; creating a new one");

    result = BusServerPtr(
            new LifecycledBusServer(
                    BusServerPtr(
                            new BusServerImpl(this->asioService, port,
                                    tcpnodelay))));
    result->activate();
    this->busServers[endpoint] = result;

    RSCDEBUG(logger, "Created new bus server " << result);

    return result;
}

BusPtr Factory::getBus(const Server&          serverMode,
                       const std::string&     host,
                       const boost::uint16_t& port,
                       bool                   tcpnodelay) {

    boost::mutex::scoped_lock lock(this->busMutex);

    switch (serverMode) {
    case SERVER_NO:
        return getBusClientFor(host, port, tcpnodelay);
    case SERVER_YES:
        return getBusServerFor(host, port, tcpnodelay);
    case SERVER_AUTO:
        try {
            return getBusServerFor(host, port, tcpnodelay);
        } catch (const std::exception& e) {
            RSCINFO(logger,
                    "Could not create server for bus: " << e.what() << "; trying to access bus as client");
            return getBusClientFor(host, port, tcpnodelay);
        }
    default:
        assert(false);
        throw invalid_argument("Impossible Server enum value received");
    }

}

void Factory::checkOptions(BusPtr bus, bool tcpnodelay) {
    if (bus->isTcpnodelay() != tcpnodelay) {
        throw invalid_argument(str(format("Requested tcpnodelay option %1% does not match existing option %2%")
                                   % tcpnodelay % bus->isTcpnodelay()));
    }
}

FactoryPtr getDefaultFactory() {
    static boost::mutex mutex;
    static FactoryPtr defaultFactory;
    boost::mutex::scoped_lock lock(mutex);
    if (!defaultFactory) {
        defaultFactory.reset(new Factory);
    }
    return defaultFactory;
}

}
}
}
