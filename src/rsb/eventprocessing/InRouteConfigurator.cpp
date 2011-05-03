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
#include "ParallelEventReceivingStrategy.h"

using namespace std;
using namespace rsc::logging;
using namespace rsb;
using namespace rsb::filter;
using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

InRouteConfigurator::InRouteConfigurator(const Scope &scope) :
    logger(Logger::getLogger("rsb.eventprocessing.InRouteConfigurator")),
            scope(scope), shutdown(false) {
}

InRouteConfigurator::~InRouteConfigurator() {
    if (!this->shutdown) {
        deactivate();
    }
}

string InRouteConfigurator::getClassName() const {
    return "InRouteConfigurator";
}

void InRouteConfigurator::printContents(ostream &stream) const {
    stream << "scope = " << scope << ", connectors = " << connectors
            << ", eventReceivingStrategy = " << eventReceivingStrategy
            << ", shutdown = " << shutdown;
}

void InRouteConfigurator::activate() {
    RSCDEBUG(logger, "Activating");

    // Activate all connectors.
    for (ConnectorList::iterator it = this->connectors.begin(); it
            != this->connectors.end(); ++it) {
        (*it)->activate();
        (*it)->setScope(scope);
    }

    // Create the event processing strategy and attach it to all
    // connectors.
    this->eventReceivingStrategy = EventReceivingStrategyPtr(
            new ParallelEventReceivingStrategy());
    for (ConnectorList::iterator it = this->connectors.begin(); it
            != this->connectors.end(); ++it) {
        (*it)->addHandler(HandlerPtr(new EventFunctionHandler(boost::bind(
                &EventReceivingStrategy::handle, this->eventReceivingStrategy,
                _1))));
    }
}

void InRouteConfigurator::deactivate() {
    RSCDEBUG(logger, "Deactivating");

    // Deactivate all connectors.
    for (ConnectorList::iterator it = this->connectors.begin(); it
            != this->connectors.end(); ++it) {
        (*it)->deactivate();
    }

    // Release event processing strategy.
    this->eventReceivingStrategy.reset();
    this->shutdown = true;
}

void InRouteConfigurator::addConnector(InConnectorPtr connector) {
    RSCDEBUG(logger, "Adding connector " << connector);
    this->connectors.push_back(connector);
}

void InRouteConfigurator::removeConnector(InConnectorPtr connector) {
    RSCDEBUG(logger, "Removing connector " << connector);
    this->connectors.remove(connector);
}

void InRouteConfigurator::handlerAdded(HandlerPtr handler) {
    eventReceivingStrategy->addHandler(handler);
}

void InRouteConfigurator::handlerRemoved(HandlerPtr handler) {
    eventReceivingStrategy->removeHandler(handler);
}

void InRouteConfigurator::filterAdded(filter::FilterPtr filter) {
    for (ConnectorList::iterator it = this->connectors.begin(); it
            != this->connectors.end(); ++it) {
        filter->notifyObserver(*it, filter::FilterAction::ADD);
    }
    eventReceivingStrategy->addFilter(filter);
}

void InRouteConfigurator::filterRemoved(filter::FilterPtr filter) {
    for (ConnectorList::iterator it = this->connectors.begin(); it
            != this->connectors.end(); ++it) {
        filter->notifyObserver(*it, filter::FilterAction::REMOVE);
    }
    eventReceivingStrategy->removeFilter(filter);
}

void InRouteConfigurator::setQualityOfServiceSpecs(const QualityOfServiceSpec &/*specs*/) {
    // TODO only required if we also want to support QoS for ingoing
    // connectors as well
    // this->inConnector->setQualityOfServiceSpecs(specs);
}

}
}
