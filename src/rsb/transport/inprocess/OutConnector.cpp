/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2012, 2013, 2015 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "OutConnector.h"

#include <boost/format.hpp>

#include <rsc/os/HostInfo.h>
#include <rsc/os/ProcessInfo.h>

#include "../../MetaData.h"

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace transport{
namespace inprocess {

OutConnector::OutConnector(BusPtr bus) :
    bus(bus) {
}

rsb::transport::OutConnector* OutConnector::create(const Properties& /*args*/) {
    // Seems to have confused some users.
    // See https://code.cor-lab.de/issues/649
    // if (args.has("converters")) {
    //     RSCWARN(logger, "`converters' property found when constructing inprocess::OutConnector. This connector does not support (or require) converters.");
    // }
    return new OutConnector();
}

void OutConnector::setScope(const Scope& /*scope*/) {
}

// Overwrites method in rsb::transport::Connector.
const std::string OutConnector::getTransportURL() const {
    return boost::str(boost::format("inprocess://%1%:%2%")
                      % rsc::os::currentHostname()
                      % rsc::os::currentProcessId());
}


void OutConnector::activate() {
}

void OutConnector::deactivate() {
}

void OutConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& /*specs*/) {
}

void OutConnector::handle(EventPtr event) {
    event->mutableMetaData().setSendTime();
    this->bus->handle(event);
}

}
}
}
