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

#include "OutRouteConfigurator.h"

#include <rsc/runtime/ContainerIO.h>
#include <rsc/logging/Logger.h>

#include "DirectEventSendingStrategy.h"
#include "../transport/OutConnector.h"
#include "../QualityOfServiceSpec.h"
#include "../transport/Connector.h"
#include "EventSendingStrategy.h"

using namespace std;

using namespace rsc::logging;

using namespace rsb;
using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

typedef std::list<transport::OutConnectorPtr> ConnectorList;

class OutRouteConfigurator::Impl {
public:

    rsc::logging::LoggerPtr logger;

    ConnectorList connectors;
    EventSendingStrategyPtr eventSendingStrategy;
    volatile bool shutdown;

};

OutRouteConfigurator::OutRouteConfigurator() :
    d(new Impl) {
    d->logger = Logger::getLogger("rsb.eventprocessing.OutRouteConfigurator");
    d->shutdown = false;
}

OutRouteConfigurator::~OutRouteConfigurator() {
    if (!d->shutdown) {
        deactivate();
    }
}

string OutRouteConfigurator::getClassName() const {
    return "OutRouteConfigurator";
}

void OutRouteConfigurator::printContents(ostream& stream) const {
    stream << "connectors = " << d->connectors << ", shutdown = "
            << d->shutdown;
}

void OutRouteConfigurator::activate() {
    RSCDEBUG(d->logger, "Activating");

    // Activate all connectors.
    for (ConnectorList::iterator it = d->connectors.begin(); it
            != d->connectors.end(); ++it) {
        (*it)->activate();
    }

    // Create a strategy object and add all connectors to it.
    d->eventSendingStrategy.reset(new DirectEventSendingStrategy());
    for (ConnectorList::iterator it = d->connectors.begin(); it
            != d->connectors.end(); ++it) {
        RSCDEBUG(d->logger, "Adding connector " << *it
                << " to strategy " << d->eventSendingStrategy);
        d->eventSendingStrategy->addConnector(*it);
    }
}

void OutRouteConfigurator::deactivate() {
    RSCDEBUG(d->logger, "Deactivating");

    // Remove all connectors from the strategy object, the release the
    // strategy.
    for (ConnectorList::iterator it = d->connectors.begin(); it
            != d->connectors.end(); ++it) {
        RSCDEBUG(d->logger, "Removing connector " << *it
                << " from strategy " << d->eventSendingStrategy);
        if (d->eventSendingStrategy) {
            d->eventSendingStrategy->removeConnector(*it);
        }
    }
    d->eventSendingStrategy.reset();

    // Deactivate all connectors.
    for (ConnectorList::iterator it = d->connectors.begin(); it
            != d->connectors.end(); ++it) {
        (*it)->deactivate();
    }

    d->shutdown = true;
}

void OutRouteConfigurator::addConnector(OutConnectorPtr connector) {
    RSCDEBUG(d->logger, "Adding connector " << connector);
    d->connectors.push_back(connector);
}

void OutRouteConfigurator::removeConnector(OutConnectorPtr connector) {
    RSCDEBUG(d->logger, "Removing connector " << connector);
    d->connectors.remove(connector);
}

void OutRouteConfigurator::publish(EventPtr e) {
    RSCDEBUG(d->logger, "OutRouteConfigurator::publish(Event) publishing new event: " << e);
    d->eventSendingStrategy->process(e);
}

void OutRouteConfigurator::setQualityOfServiceSpecs(
        const QualityOfServiceSpec& specs) {
    for (ConnectorList::iterator it = d->connectors.begin(); it
            != d->connectors.end(); ++it) {
        (*it)->setQualityOfServiceSpecs(specs);
    }
}

}
}
