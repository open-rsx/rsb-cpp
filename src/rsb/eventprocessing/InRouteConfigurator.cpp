/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "InRouteConfigurator.h"

#include "../Factory.h"
#include "../ParticipantConfig.h"
#include "../QualityOfServiceSpec.h"
#include "../filter/Filter.h"
#include "EventReceivingStrategy.h"

using namespace std;
using namespace rsc::logging;
using namespace rsb;
using namespace rsb::filter;
using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

class InRouteConfigurator::Impl {
public:

    Impl(
            const ParticipantConfig::EventProcessingStrategy& receivingStrategyConfig) :
        receivingStrategyConfig(receivingStrategyConfig) {

    }

    rsc::logging::LoggerPtr logger;

    ParticipantConfig::EventProcessingStrategy receivingStrategyConfig;

    Scope scope;
    ConnectorSet connectors;
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
            << ", shutdown = " << d->shutdown;
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
    d->eventReceivingStrategy = createEventReceivingStrategy();
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

const ParticipantConfig::EventProcessingStrategy& InRouteConfigurator::getReceivingStrategyConfig() const {
    return d->receivingStrategyConfig;
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

void InRouteConfigurator::setQualityOfServiceSpecs(
        const QualityOfServiceSpec& specs) {
    for (ConnectorSet::iterator it = d->connectors.begin(); it
            != d->connectors.end(); ++it) {
        (*it)->setQualityOfServiceSpecs(specs);
    }
}

}
}
