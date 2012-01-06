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

#include "DirectEventSendingStrategy.h"

#include <rsc/runtime/ContainerIO.h>

#include "../transport/OutConnector.h"

using namespace std;

using namespace rsc::runtime;

namespace rsb {
namespace eventprocessing {

EventSendingStrategy* DirectEventSendingStrategy::create(const Properties& props) {
    return new DirectEventSendingStrategy();
}

void DirectEventSendingStrategy::printContents(ostream& stream) const {
    stream << "connectors = " << connectors;
}

void DirectEventSendingStrategy::addConnector(transport::OutConnectorPtr connector) {
    this->connectors.push_back(connector);
}

void DirectEventSendingStrategy::removeConnector(transport::OutConnectorPtr connector) {
    this->connectors.remove(connector);
}

void DirectEventSendingStrategy::process(EventPtr event) {
    for (ConnectorList::const_iterator it = this->connectors.begin();
         it != this->connectors.end(); ++it) {
        (*it)->handle(event);
    }
}

}
}
