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

#include "Reader.h"

#include "eventprocessing/PullEventReceivingStrategy.h"
#include "eventprocessing/PullInRouteConfigurator.h"

using namespace std;

using namespace boost;

using namespace rsb::eventprocessing;
using namespace rsb::transport;

namespace rsb {

Reader::Reader(const vector<InPullConnectorPtr>& connectors,
               const Scope&                  scope,
               const ParticipantConfig&      config) :
    Participant(scope, config) {
    this->configurator.reset(new PullInRouteConfigurator(scope, config));
    for (vector<InPullConnectorPtr>::const_iterator it = connectors.begin(); it
             != connectors.end(); ++it) {
        this->configurator->addConnector(*it);
    }
    this->configurator->activate();
}

EventPtr Reader::read(bool block) {
    PullEventReceivingStrategyPtr strategy
        = dynamic_pointer_cast<PullEventReceivingStrategy>(this->configurator->getEventReceivingStrategy());
    assert(strategy);
    return strategy->raiseEvent(block);
}

string Reader::getClassName() const {
    return "Reader";
}

}
