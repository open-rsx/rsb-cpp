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

#include "DirectEventSendingStrategy.h"

using namespace std;

using namespace rsc::logging;

using namespace rsb;
using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

OutRouteConfigurator::OutRouteConfigurator(OutConnectorPtr outConnector) :
    logger(Logger::getLogger("rsb.eventprocessing.OutRouteConfigurator")),
    outConnector(outConnector), shutdown(false) {
}

OutRouteConfigurator::~OutRouteConfigurator() {
    if (!this->shutdown) {
        deactivate();
    }
}

void OutRouteConfigurator::activate() {
    RSCDEBUG(logger, "Activating connector " << this->outConnector);
    this->outConnector->activate();

    this->eventSendingStrategy.reset(new DirectEventSendingStrategy());
    RSCDEBUG(logger, "Adding connector " << this->outConnector
             << " to strategy " << this->eventSendingStrategy);
    this->eventSendingStrategy->addConnector(this->outConnector);
}

void OutRouteConfigurator::deactivate() {
    RSCDEBUG(logger, "Removing connector " << this->outConnector
             << " from strategy " << this->eventSendingStrategy);
    this->eventSendingStrategy->removeConnector(this->outConnector);
    this->eventSendingStrategy.reset();

    RSCDEBUG(logger, "Deactivating connector " << this->outConnector);
    this->outConnector->deactivate();
    this->shutdown = true;
}

void OutRouteConfigurator::publish(EventPtr e) {
    RSCDEBUG(logger, "OutRouteConfigurator::publish(Event) publishing new event: " << *e);
    this->eventSendingStrategy->process(e);
}

void OutRouteConfigurator::setQualityOfServiceSpecs(const QualityOfServiceSpec &specs) {
    if (this->outConnector) {
        this->outConnector->setQualityOfServiceSpecs(specs);
    }
}

}
}
