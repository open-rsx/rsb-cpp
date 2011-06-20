/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "PushInRouteConfigurator.h"

#include "ParallelEventReceivingStrategy.h"

using namespace std;

using namespace boost;

using namespace rsc::logging;

using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

PushInRouteConfigurator::PushInRouteConfigurator(const Scope &scope) :
    InRouteConfigurator(scope),
    logger(Logger::getLogger("rsb.eventprocessing.PushInRouteConfigurator")),
    errorStrategy(ParticipantConfig::LOG) {
}

PushInRouteConfigurator::~PushInRouteConfigurator() {
}

string PushInRouteConfigurator::getClassName() const {
    return "PushInRouteConfigurator";
}

void PushInRouteConfigurator::printContents(ostream &stream) const {
    InRouteConfigurator::printContents(stream);
    stream << ", errorStrategy = " << this->errorStrategy;
}

void PushInRouteConfigurator::activate() {
    InRouteConfigurator::activate();

    // The base class has instantiated an
    // EventReceivingStrategy. Retrieve it and install our error
    // handling strategy.
    this->eventReceivingStrategy
        = dynamic_pointer_cast<PushEventReceivingStrategy>(getEventReceivingStrategy());
    this->eventReceivingStrategy->setHandlerErrorStrategy(this->errorStrategy);

    // Retrieve the set of connectors and ourselves to the list of
    // handlers of each connector.
    InRouteConfigurator::ConnectorSet connectors = getConnectors();
    for (InRouteConfigurator::ConnectorSet::const_iterator it = connectors.begin();
	 it != connectors.end(); ++it) {
	InPushConnectorPtr connector = dynamic_pointer_cast<InPushConnector>(*it);
	assert(connector);
	connector->addHandler(HandlerPtr(new EventFunctionHandler(boost::bind(
								      &PushEventReceivingStrategy::handle,
								      this->eventReceivingStrategy,
								      _1))));
    }
}

void PushInRouteConfigurator::handlerAdded(rsb::HandlerPtr handler, const bool &wait) {
    this->eventReceivingStrategy->addHandler(handler, wait);
}

void PushInRouteConfigurator::handlerRemoved(rsb::HandlerPtr handler, const bool &wait) {
    this->eventReceivingStrategy->removeHandler(handler, wait);
}

void PushInRouteConfigurator::setErrorStrategy(const ParticipantConfig::ErrorStrategy &strategy) {
    if (this->eventReceivingStrategy) {
        this->eventReceivingStrategy->setHandlerErrorStrategy(strategy);
    }
    this->errorStrategy = strategy;
}

EventReceivingStrategyPtr PushInRouteConfigurator::createEventReceivingStrategy() {
    return EventReceivingStrategyPtr(new ParallelEventReceivingStrategy());
}

}
}
