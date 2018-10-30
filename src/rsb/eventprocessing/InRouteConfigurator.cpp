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

#include "InRouteConfigurator.h"

#include <boost/bind.hpp>

#include "../Factory.h"
#include "../ParticipantConfig.h"
#include "../QualityOfServiceSpec.h"
#include "../Handler.h"
#include "../filter/Filter.h"

#include "EventReceivingStrategy.h"
#include "EventReceivingStrategyFactory.h"

using namespace std;
using namespace rsc::logging;
using namespace rsb;
using namespace rsb::filter;
using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

class InRouteConfigurator::Impl {
public:

    Impl(const ParticipantConfig::EventProcessingStrategy& receivingStrategyConfig) :        
        receivingStrategyConfig(receivingStrategyConfig),
        errorStrategy(ParticipantConfig::ERROR_STRATEGY_LOG) {
    }

    rsc::logging::LoggerPtr logger;

    ParticipantConfig::EventProcessingStrategy receivingStrategyConfig;

    Scope scope;
    ConnectorSet connectors;
    ParticipantConfig::ErrorStrategy errorStrategy;
    EventReceivingStrategyPtr eventReceivingStrategy;
    volatile bool shutdown;
};

InRouteConfigurator::InRouteConfigurator(const Scope& scope,
        const ParticipantConfig& config) :
    d(new Impl(config.getEventReceivingStrategy())) {
    d->logger = Logger::getLogger("rsb.eventprocessing.InRouteConfigurator");
    d->scope = scope;
    d->shutdown = false;
}

InRouteConfigurator::~InRouteConfigurator() {
    if (!d->shutdown) {
        deactivate();
    }
}

string InRouteConfigurator::getClassName() const {
    return "InRouteConfigurator";
}

void InRouteConfigurator::printContents(ostream& stream) const {
    stream << "scope = " << d->scope << ", connectors = " << d->connectors
           << ", eventReceivingStrategy = " << d->eventReceivingStrategy
           << ", errorStrategy = " << d->errorStrategy
           << ", shutdown = " << d->shutdown;
}

const std::set<std::string> InRouteConfigurator::getTransportURLs() const {
    std::set<std::string> result;
    for (ConnectorSet::iterator it = d->connectors.begin();
         it != d->connectors.end(); ++it) {
        result.insert((*it)->getTransportURL());
    }
    return result;
}

void InRouteConfigurator::activate() {
    RSCDEBUG(d->logger, "Activating");

    // Activate all connectors.
    for (ConnectorSet::iterator it = d->connectors.begin(); it
            != d->connectors.end(); ++it) {
        (*it)->setScope(d->scope);
        (*it)->activate();
    }

    // Create the event processing strategy and attach it to all
    // connectors.
    {
      std::string impl = d->receivingStrategyConfig.getName();
        rsc::runtime::Properties options = d->receivingStrategyConfig.getOptions();
        RSCDEBUG(d->logger, "Instantiating event receiving strategy with config "
                 << d->receivingStrategyConfig);
        d->eventReceivingStrategy
          .reset(getEventReceivingStrategyFactory().createInst(impl, options));
        // Install our error // handling strategy.
        d->eventReceivingStrategy->setHandlerErrorStrategy(d->errorStrategy);
    }

    // Retrieve the set of connectors and ourselves to the list of
    // handlers of each connector.
    ConnectorSet connectors = getConnectors();
    for (ConnectorSet::const_iterator it =
           connectors.begin(); it != connectors.end(); ++it) {
      (*it)->setErrorStrategy(d->errorStrategy);
      (*it)->addHandler(
                        HandlerPtr(
                                   new EventFunctionHandler(
                                                            boost::bind(&EventReceivingStrategy::handle,
                                                                        d->eventReceivingStrategy, _1))));
    }
}

void InRouteConfigurator::deactivate() {
    RSCDEBUG(d->logger, "Deactivating");

    // Deactivate all connectors.
    for (ConnectorSet::iterator it = d->connectors.begin(); it
            != d->connectors.end(); ++it) {
        (*it)->deactivate();
    }

    // Release event processing strategy.
    d->eventReceivingStrategy.reset();
    d->shutdown = true;
}

EventReceivingStrategyPtr InRouteConfigurator::getEventReceivingStrategy() const {
    return d->eventReceivingStrategy;
}

InRouteConfigurator::ConnectorSet InRouteConfigurator::getConnectors() {
    return d->connectors;
}

void InRouteConfigurator::addConnector(InConnectorPtr connector) {
    RSCDEBUG(d->logger, "Adding connector " << connector);
    d->connectors.insert(connector);
}

void InRouteConfigurator::removeConnector(InConnectorPtr connector) {
    RSCDEBUG(d->logger, "Removing connector " << connector);
    d->connectors.erase(connector);
}

void InRouteConfigurator::filterAdded(filter::FilterPtr filter) {
    for (ConnectorSet::iterator it = d->connectors.begin(); it
            != d->connectors.end(); ++it) {
        filter->notifyObserver(*it, filter::FilterAction::ADD);
    }
    d->eventReceivingStrategy->addFilter(filter);
}

void InRouteConfigurator::filterRemoved(filter::FilterPtr filter) {
    for (ConnectorSet::iterator it = d->connectors.begin(); it
            != d->connectors.end(); ++it) {
        filter->notifyObserver(*it, filter::FilterAction::REMOVE);
    }
    d->eventReceivingStrategy->removeFilter(filter);
}

void InRouteConfigurator::handlerAdded(rsb::HandlerPtr handler, const bool& wait) {
    d->eventReceivingStrategy->addHandler(handler, wait);
}

void InRouteConfigurator::handlerRemoved(rsb::HandlerPtr handler, const bool& wait) {
    d->eventReceivingStrategy->removeHandler(handler, wait);
}

void InRouteConfigurator::setErrorStrategy(const ParticipantConfig::ErrorStrategy& strategy) {
    if (d->eventReceivingStrategy) {
      d->eventReceivingStrategy->setHandlerErrorStrategy(strategy);
    }
    ConnectorSet connectors = getConnectors();
    for (ConnectorSet::const_iterator it =
           connectors.begin(); it != connectors.end(); ++it) {
      (*it)->setErrorStrategy(strategy);
    }
    d->errorStrategy = strategy;
}

void InRouteConfigurator::setQualityOfServiceSpecs(
        const QualityOfServiceSpec& specs) {
    for (ConnectorSet::iterator it = d->connectors.begin(); it
            != d->connectors.end(); ++it) {
        (*it)->setQualityOfServiceSpecs(specs);
    }
}

}
}
