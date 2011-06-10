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

#include "OutRouteConfigurator.h"

#include <rsc/runtime/ContainerIO.h>

#include "DirectEventSendingStrategy.h"

using namespace std;

using namespace rsc::logging;

using namespace rsb;
using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

OutRouteConfigurator::OutRouteConfigurator() :
    logger(Logger::getLogger("rsb.eventprocessing.OutRouteConfigurator")),
            shutdown(false) {
}

OutRouteConfigurator::~OutRouteConfigurator() {
    if (!this->shutdown) {
        deactivate();
    }
}

string OutRouteConfigurator::getClassName() const {
    return "OutRouteConfigurator";
}

void OutRouteConfigurator::printContents(ostream &stream) const {
    stream << "connectors = " << connectors << ", shutdown = " << shutdown;
}

void OutRouteConfigurator::activate() {
    RSCDEBUG(logger, "Activating");

    // Activate all connectors.
    for (ConnectorList::iterator it = this->connectors.begin(); it
            != this->connectors.end(); ++it) {
        (*it)->activate();
    }

    // Create a strategy object and add all connectors to it.
    this->eventSendingStrategy.reset(new DirectEventSendingStrategy());
    for (ConnectorList::iterator it = this->connectors.begin(); it
            != this->connectors.end(); ++it) {
        RSCDEBUG(logger, "Adding connector " << *it
                << " to strategy " << this->eventSendingStrategy);
        this->eventSendingStrategy->addConnector(*it);
    }
}

void OutRouteConfigurator::deactivate() {
    RSCDEBUG(logger, "Deactivating");

    // Remove all connectors from the strategy object, the release the
    // strategy.
    for (ConnectorList::iterator it = this->connectors.begin(); it
            != this->connectors.end(); ++it) {
        RSCDEBUG(logger, "Removing connector " << *it
                << " from strategy " << this->eventSendingStrategy);
        this->eventSendingStrategy->removeConnector(*it);
    }
    this->eventSendingStrategy.reset();

    // Deactivate all connectors.
    for (ConnectorList::iterator it = this->connectors.begin(); it
            != this->connectors.end(); ++it) {
        (*it)->deactivate();
    }

    this->shutdown = true;
}

void OutRouteConfigurator::addConnector(OutConnectorPtr connector) {
    RSCDEBUG(logger, "Adding connector " << connector);
    this->connectors.push_back(connector);
}

void OutRouteConfigurator::removeConnector(OutConnectorPtr connector) {
    RSCDEBUG(logger, "Removing connector " << connector);
    this->connectors.remove(connector);
}

void OutRouteConfigurator::publish(EventPtr e) {
    RSCDEBUG(logger, "OutRouteConfigurator::publish(Event) publishing new event: " << *e);
    this->eventSendingStrategy->process(e);
}

void OutRouteConfigurator::setQualityOfServiceSpecs(
        const QualityOfServiceSpec &specs) {
    for (ConnectorList::iterator it = this->connectors.begin(); it
            != this->connectors.end(); ++it) {
        (*it)->setQualityOfServiceSpecs(specs);
    }
}

}
}
