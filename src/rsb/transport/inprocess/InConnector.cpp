/* ============================================================
 *
 * This file is part of the RSB project.
 *
 * Copyright (C) 2012 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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
 * ============================================================  */

#include "InConnector.h"

using namespace std;

using namespace rsc::logging;

namespace rsb {
namespace transport{
namespace inprocess {

InConnector::InConnector(BusPtr bus) :
    logger(Logger::getLogger("rsb.transport.inprocess.InConnector")),
    bus(bus),
    active(false) {
}

InConnector::~InConnector() {
    if (this->active) {
        deactivate();
    }
}

void InConnector::printContents(ostream& stream) const {
    stream << "scope = " << scope;
}

Scope InConnector::getScope() const {
    return this->scope;
}

void InConnector::setScope(const Scope& scope) {
    if (this->active) {
        throw std::runtime_error("Cannot set scope while active");
    }

    this->scope = scope;
}

void InConnector::activate() {
    RSCDEBUG(logger, "Activating");

    bus->addSink(boost::dynamic_pointer_cast<InConnector>(shared_from_this()));

    this->active = true;
}

void InConnector::deactivate() {
    RSCDEBUG(logger, "Deactivating");

    this->active = false;

    bus->removeSink(this);
}

}
}
}
