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

InRouteConfigurator::InRouteConfigurator() :
    logger(Logger::getLogger("rsb.eventprocessing.InRouteConfigurator")),
    shutdown(false) {
}

InRouteConfigurator::~InRouteConfigurator() {
    if (!this->shutdown) {
        deactivate();
    }
}

void InRouteConfigurator::activate() {
    RSCDEBUG(logger, "Activating");

    for (ConnectorList::iterator it = this->connectors.begin();
         it != this->connectors.end(); ++it) {
        (*it)->activate();
    }

    this->eventReceivingStrategy = EventReceivingStrategyPtr(new ParallelEventReceivingStrategy());

    // add event processor as observer to connectors
    for (ConnectorList::iterator it = this->connectors.begin();
         it != this->connectors.end(); ++it) {
        (*it)->addHandler(HandlerPtr(new EventFunctionHandler(boost::bind(&EventReceivingStrategy::handle,
                                                                          this->eventReceivingStrategy, _1))));
    }
}

void InRouteConfigurator::deactivate() {
    for (ConnectorList::iterator it = this->connectors.begin();
         it != this->connectors.end(); ++it) {
        (*it)->deactivate();
    }

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

void InRouteConfigurator::notifyConnectors(SubscriptionPtr s,
                              filter::FilterAction::Types a) {

    for (ConnectorList::iterator it = this->connectors.begin();
         it != this->connectors.end(); ++it) {
        FilterObserverPtr fo = boost::static_pointer_cast<FilterObserver>(*it);
        for (FilterChain::iterator listIt = s->getFilters()->begin(); listIt
                 != s->getFilters()->end(); ++listIt) {
            // TODO check whether we want to do this also for out ports
            // TODO generally use filters::Observable implementation here!
            (*listIt)->notifyObserver(fo, a);
        }
    }
}

void InRouteConfigurator::subscribe(SubscriptionPtr s, set<HandlerPtr> handlers) {
    // notify ports about new subscription
    notifyConnectors(s, filter::FilterAction::ADD);
    this->eventReceivingStrategy->subscribe(s, handlers);
}

void InRouteConfigurator::unsubscribe(SubscriptionPtr s) {
    // notify ports about removal of subscription
    notifyConnectors(s, filter::FilterAction::REMOVE);
    this->eventReceivingStrategy->unsubscribe(s);
}

void InRouteConfigurator::setQualityOfServiceSpecs(const QualityOfServiceSpec &/*specs*/) {
    // TODO only required if we also want to support QoS for ingoing
    // connectors as well
    // this->inConnector->setQualityOfServiceSpecs(specs);
}

}
}
