/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011-2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "PullInRouteConfigurator.h"

#include <set>

#include "../transport/InConnector.h"
#include "PullEventReceivingStrategy.h"

using namespace std;

using namespace boost;

using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

PullInRouteConfigurator::PullInRouteConfigurator(const Scope&             scope,
                                                 const ParticipantConfig& config) :
    InRouteConfigurator(scope, config) {
}

EventReceivingStrategyPtr PullInRouteConfigurator::createEventReceivingStrategy() {
    // Pass all configured connectors a PullEventReceivingStrategy
    // object, ensuring that they actually are InPullConnectors.
    set<InConnectorPtr> connectors_ = getConnectors();
    set<InConnectorPtr> connectors;
    for (set<InConnectorPtr>::const_iterator it = connectors_.begin();
         it != connectors_.end(); ++it) {
        InConnectorPtr connector = dynamic_pointer_cast<InConnector>(*it);
        assert(connector);
        connectors.insert(connector);
    }
    return EventReceivingStrategyPtr(new PullEventReceivingStrategy(connectors));
}

string PullInRouteConfigurator::getClassName() const {
    return "PullInRouteConfigurator";
}

}
}
