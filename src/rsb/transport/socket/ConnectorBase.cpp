/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2012, 2013 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "ConnectorBase.h"

#include <boost/format.hpp>

#include "Bus.h"
#include "Factory.h"

using namespace std;

using namespace boost;

using namespace rsc::logging;

namespace rsb {
namespace transport {
namespace socket {

ConnectorBase::ConnectorBase(FactoryPtr                    factory,
                             ConverterSelectionStrategyPtr converters,
                             const string&                 host,
                             unsigned int                  port,
                             Server                        server,
                             bool                          tcpnodelay) :
    ConverterSelectingConnector<string>(converters),
    active(false), logger(Logger::getLogger("rsb.transport.socket.ConnectorBase")),
    factory(factory), host(host), port(port), server(server),
    tcpnodelay(tcpnodelay) {
}

ConnectorBase::~ConnectorBase() {
    RSCDEBUG(logger, "Destructing");
    if (this->active) {
        deactivate();
    }
    RSCDEBUG(logger, "Destruction finished");
}

Scope ConnectorBase::getScope() const {
    return this->scope;
}

void ConnectorBase::setScope(const Scope& scope) {
    if (this->active)
        throw std::runtime_error("Cannot set scope while active");

    this->scope = scope;
}

void ConnectorBase::activate() {
    RSCDEBUG(logger, "Activating");

    // This connector is added to the connector list of the bus returned by
    // getBus
    RSCINFO(logger, "Server mode: " << this->server);
    this->bus = this->factory->getBus(this->server, this->host, this->port,
            this->tcpnodelay);

    this->active = true;

    RSCDEBUG(logger, "Using bus " << getBus());
}

void ConnectorBase::deactivate() {
    RSCDEBUG(logger, "Deactivating");

    this->active = false;

    BusPtr bus = getBus();
    this->bus.reset();
}

BusPtr ConnectorBase::getBus() {
    return this->bus;
}

}
}
}
