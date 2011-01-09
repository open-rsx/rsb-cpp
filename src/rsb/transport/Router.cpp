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

using namespace std;
using namespace rsc::logging;
using namespace rsb;
using namespace rsb::filter;
using namespace rsb::internal;
using namespace rsb::transport;

namespace rsb {
namespace transport {

Router::Router(TransportFactory::PortTypes inType,
		TransportFactory::PortTypes outType) :
	logger(Logger::getLogger("rsb.transport.Router")) {
	inPort = TransportFactory::createPort(inType);
	outPort = TransportFactory::createPort(outType);
	if (inPort) {
		eventProcessor = EventProcessorPtr(new EventProcessor());
		// add event processor as observer to input port(s)
		inPort->setObserver(boost::bind(&EventProcessor::process,
				eventProcessor, _1));
	}
	shutdown = false;
}

void Router::activate() {
	if (inPort) {
		inPort->activate();
	}
	if (outPort) {
		outPort->activate();
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

void Router::publish(RSBEventPtr e) {
	RSCDEBUG(logger, "Router::publish(RSBEvent) publishing new event: " << e);
	outPort->push(e);
}

void Router::notifyPorts(rsb::SubscriptionPtr s,
		rsb::filter::FilterAction::Types a) {
	// TODO missing check if there really is an inport
	FilterObserverPtr fo = boost::static_pointer_cast<FilterObserver>(inPort);
	for (rsb::FilterChain::iterator list_iter = s->getFilters()->begin(); list_iter
			!= s->getFilters()->end(); list_iter++) {
		// TODO check whether we want to do this also for out ports
		// TODO generally use rsb::filters::Observable implementation here!
		(*list_iter)->notifyObserver(fo, a);
	}
}

void Router::subscribe(rsb::SubscriptionPtr s) {
	// notify ports about new subscription
	notifyPorts(s, rsb::filter::FilterAction::ADD);
	// TODO missing check if there really is an inport and ep
	eventProcessor->subscribe(s);
}

void Router::unsubscribe(rsb::SubscriptionPtr s) {
	// notify ports about removal of subscription
	notifyPorts(s, rsb::filter::FilterAction::REMOVE);
	// TODO missing check if there really is an inport and ep
	eventProcessor->unsubscribe(s);
}

void Router::setQualityOfServiceSpecs(const QualityOfServiceSpec &specs) {
	// TODO only required if we also want to support QoS for ingoing ports
	// aswell
	//	if (inPort) {
	//		inPort->setQualityOfServiceSpecs(specs);
	//	}
	if (outPort) {
		outPort->setQualityOfServiceSpecs(specs);
	}
}

}
}

