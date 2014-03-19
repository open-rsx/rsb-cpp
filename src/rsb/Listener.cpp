/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include "Listener.h"

#include <algorithm>
#include <iterator>

#include "eventprocessing/InRouteConfigurator.h"
#include "eventprocessing/PushInRouteConfigurator.h"

using namespace std;

using namespace rsb::transport;

namespace rsb {

class Listener::Impl {
public:
    rsc::logging::LoggerPtr logger;
    eventprocessing::PushInRouteConfiguratorPtr configurator;
};

Listener::Listener(const vector<InPushConnectorPtr>& connectors,
        const Scope& scope, const ParticipantConfig& config) :
    Participant(scope, config), d(new Impl) {

    d->logger = rsc::logging::Logger::getLogger("rsb.Listener");
    this->initialize(connectors, scope, config);
}

Listener::~Listener() {
}

std::string Listener::getKind() const {
    return "listener";
}

void Listener::initialize(const vector<InPushConnectorPtr>& connectors,
        const Scope& scope, const ParticipantConfig& config) {
    d->configurator.reset(
            new eventprocessing::PushInRouteConfigurator(scope, config));
    d->configurator->setErrorStrategy(getConfig().getErrorStrategy());
    for (vector<InPushConnectorPtr>::const_iterator it = connectors.begin(); it
            != connectors.end(); ++it) {
        d->configurator->addConnector(*it);
    }

    d->configurator->activate();
}

void Listener::addHandler(HandlerPtr h, bool wait) {
    d->configurator->handlerAdded(h, wait);
}

void Listener::removeHandler(HandlerPtr h, bool wait) {
    d->configurator->handlerRemoved(h, wait);
}

void Listener::addFilter(filter::FilterPtr filter) {
    d->configurator->filterAdded(filter);
}

void Listener::removeFilter(filter::FilterPtr filter) {
    d->configurator->filterRemoved(filter);
}

}
