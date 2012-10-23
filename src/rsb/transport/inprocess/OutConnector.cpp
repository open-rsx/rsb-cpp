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

#include "OutConnector.h"

#include "../../MetaData.h"

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace inprocess {

OutConnector::OutConnector() :
    bus(Bus::getInstance()) {
}

rsb::transport::OutConnector* OutConnector::create(const Properties& args) {
    LoggerPtr logger = Logger::getLogger("rsb.inprocess.OutConnector");
    RSCDEBUG(logger, "Creating OutConnector with properties " << args);

    // Seems to have confused some users.
    // See https://code.cor-lab.de/issues/649
    // if (args.has("converters")) {
    //     RSCWARN(logger, "`converters' property found when constructing inprocess::OutConnector. This connector does not support (or require) converters.");
    // }
    return new OutConnector();
}

void OutConnector::setScope(const Scope& /*scope*/) {
}

void OutConnector::activate() {
}

void OutConnector::deactivate() {
}

void OutConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& /*specs*/) {
}

void OutConnector::handle(EventPtr event) {
    event->mutableMetaData().setSendTime();
    this->bus.handle(event);
}

}
}
