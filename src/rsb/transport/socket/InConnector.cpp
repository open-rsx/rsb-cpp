/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2012, 2013 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "InConnector.h"

#include "../../MetaData.h"

#include "Factory.h"

using namespace std;

using namespace boost;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace transport {
namespace socket {

InConnector::InConnector(ConverterSelectionStrategyPtr converters,
                         const string&                 host,
                         unsigned int                  port,
                         Server                        server,
                         bool                          tcpnodelay) :
    ConnectorBase(converters, host, port, server, tcpnodelay),
    logger(Logger::getLogger("rsb.transport.socket.InConnector")) {
}

InConnector::~InConnector() {
    if (this->active) {
        deactivate();
    }
}

void InConnector::activate() {
    ConnectorBase::activate();

    RSCDEBUG(logger, "Activating");

    getBus()->addSink(dynamic_pointer_cast<InConnector>(enable_shared_from_this<rsb::transport::InConnector>::shared_from_this()));
}

void InConnector::deactivate() {
    RSCDEBUG(logger, "Deactivating");

    BusPtr bus = getBus();
    if (bus) {
        RSCDEBUG(logger, "Removing ourselves from sink list of bus " << bus);
        bus->removeSink(this);
    }

    ConnectorBase::deactivate();
}

void InConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& /*specs*/) {
    RSCWARN(logger, "Quality of service not implemented");
}

void InConnector::printContents(ostream& stream) const {
    stream << "scope = " << getScope();
}

}
}
}
