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

#include "PullEventReceivingStrategy.h"

#include "../filter/Filter.h"

#include "../transport/InPullConnector.h"

using namespace std;

using namespace boost;

using namespace rsb::filter;
using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

typedef std::set<transport::InPullConnectorPtr> ConnectorSet;
typedef std::set<filter::FilterPtr> FilterSet;

class PullEventReceivingStrategy::Impl {
public:
    FilterSet filters;

    ConnectorSet connectors;
    /**
     * Stores event obtained via connector callbacks.
     * */
    EventPtr currentEvent;
};

PullEventReceivingStrategy::PullEventReceivingStrategy(
        const set<InPullConnectorPtr>& connectors) : d(new Impl) {
    d->connectors = connectors;
}

PullEventReceivingStrategy::~PullEventReceivingStrategy() {
}

void PullEventReceivingStrategy::addFilter(FilterPtr filter) {
    d->filters.insert(filter);
}

void PullEventReceivingStrategy::removeFilter(FilterPtr filter) {
    d->filters.erase(filter);
}

EventPtr PullEventReceivingStrategy::raiseEvent(bool block) {
    // Go through our connectors and ask them to emit an event. If one
    // connector does emit an event, our handle method gets called and
    // may store the event if it matches our filters.
    for (ConnectorSet::iterator it = d->connectors.begin(); it
            != d->connectors.end(); ++it) {
        if ((*it)->raiseEvent(block)) {
            if (d->currentEvent) {
                EventPtr result = d->currentEvent;
                d->currentEvent.reset();
                return result;
            }
        }
    }
    return EventPtr();
}

void PullEventReceivingStrategy::handle(EventPtr event) {
    // TODO filter
    d->currentEvent = event;
}

std::string PullEventReceivingStrategy::getClassName() const {
    return "PullEventReceivingStrategy";
}

void PullEventReceivingStrategy::printContents(ostream& stream) const {
    stream << "connectors = " << d->connectors << ", filters = "
            << d->filters;
}

}
}
