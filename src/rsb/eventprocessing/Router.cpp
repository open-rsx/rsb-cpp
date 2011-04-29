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

#include "Router.h"

#include <rsc/logging/Logger.h>

#include "../Factory.h"
#include "ParallelEventProcessingStrategy.h"

using namespace std;
using namespace rsc::logging;
using namespace rsb;
using namespace rsb::filter;
using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

Router::Router(transport::Factory::ConnectorTypes inType,
               transport::Factory::ConnectorTypes outType) :
logger(Logger::getLogger("rsb.transport.Router")) {
    // TODO workaround until Router gets refactored
    rsb::Factory::getInstance();
    switch (inType) {
    case transport::Factory::LOCAL:
      this->inConnector.reset(transport::InFactory::getInstance().createInst("inprocess"));
      break;
    case transport::Factory::SPREAD:
      this->inConnector.reset(transport::InFactory::getInstance().createInst("spread"));
      break;
    case transport::Factory::NONE:
      break;
    }
    switch (outType) {
    case transport::Factory::LOCAL:
      this->outConnector.reset(transport::OutFactory::getInstance().createInst("inprocess"));
      break;
    case transport::Factory::SPREAD:
      this->outConnector.reset(transport::OutFactory::getInstance().createInst("spread"));
      break;
    case transport::Factory::NONE:
      break;
    }
    if (this->inConnector) {
      eventProcessingStrategy = EventProcessingStrategyPtr(new ParallelEventProcessingStrategy());
      // add event processor as observer to input port(s)
      inConnector->setObserver(HandlerPtr(new EventFunctionHandler(boost::bind(&EventProcessingStrategy::process, eventProcessingStrategy, _1))));
    }
    shutdown = false;
}

void Router::activate() {
	if (inConnector) {
		inConnector->activate();
	}
	if (outConnector) {
		outConnector->activate();
	}
}

void Router::deactivate() {
	shutdown = true;
	// TODO
}

Router::~Router() {
	if (!shutdown) {
		deactivate();
	}
}

void Router::publish(EventPtr e) {
	RSCDEBUG(logger, "Router::publish(Event) publishing new event: " << e);
	outConnector->push(e);
}

void Router::notifyConnectors(rsb::SubscriptionPtr s,
		rsb::filter::FilterAction::Types a) {
	// TODO missing check if there really is an inport
	FilterObserverPtr fo = boost::static_pointer_cast<FilterObserver>(inConnector);
	for (rsb::FilterChain::iterator listIt = s->getFilters()->begin(); listIt
			!= s->getFilters()->end(); ++listIt) {
		// TODO check whether we want to do this also for out ports
		// TODO generally use rsb::filters::Observable implementation here!
		(*listIt)->notifyObserver(fo, a);
	}
}

void Router::subscribe(rsb::SubscriptionPtr s, set<HandlerPtr> handlers) {
	// notify ports about new subscription
	notifyConnectors(s, rsb::filter::FilterAction::ADD);
	// TODO missing check if there really is an inport and ep
	eventProcessingStrategy->subscribe(s, handlers);
}

void Router::unsubscribe(rsb::SubscriptionPtr s) {
	// notify ports about removal of subscription
	notifyConnectors(s, rsb::filter::FilterAction::REMOVE);
	// TODO missing check if there really is an inport and ep
	eventProcessingStrategy->unsubscribe(s);
}

void Router::setQualityOfServiceSpecs(const QualityOfServiceSpec &specs) {
	// TODO only required if we also want to support QoS for ingoing ports
	// aswell
	//	if (inConnector) {
	//		inConnector->setQualityOfServiceSpecs(specs);
	//	}
	if (outConnector) {
		outConnector->setQualityOfServiceSpecs(specs);
	}
}

ConnectorPtr Router::getOutConnector() {
	return outConnector;
}

ConnectorPtr Router::getInConnector() {
	return inConnector;
}

}
}
