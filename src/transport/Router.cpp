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

#include <log4cxx/logger.h>
#include <log4cxx/stream.h>
#include <log4cxx/level.h>

using namespace std;
using namespace log4cxx;
using namespace rsb;
using namespace rsb::filter;
using namespace rsb::internal;
using namespace rsb::transport;

namespace rsb {

namespace transport {

Router::Router(TransportFactory::PortTypes inType, TransportFactory::PortTypes outType) : logger(Logger::getLogger("rsb.transport.Router")) {
	ip = TransportFactory::createPort(inType);
	op = TransportFactory::createPort(outType);
	ep = EventProcessorPtr(new EventProcessor());
	// add event processor as observer to input port(s)
	ip->setObserver(boost::bind(&EventProcessor::process, ep.get(), _1));
	shutdown = false;
}

void Router::activate() {
	ip->activate();
	op->activate();
}

void Router::deactivate(){
	shutdown = true;
	// TODO
}

Router::~Router() {
	if (!shutdown) deactivate();
}

void Router::publish(RSBEventPtr e) {
	LOG4CXX_DEBUG(logger, "Router::publish(RSBEvent) publishing new event: " << e);
	op->push(e);
}

void Router::notifyPorts(rsb::SubscriptionPtr s, rsb::filter::FilterAction::Types a) {
		FilterObserverPtr fo = boost::static_pointer_cast<FilterObserver>(ip);
	for(rsb::FilterChain::iterator list_iter = s->getFilters()->begin();
	    list_iter != s->getFilters()->end(); list_iter++)
	{
		// TODO check whether we want to do this also for out ports
		// TODO generally use rsb::filters::Observable implementation here!
		(*list_iter)->notifyObserver(fo,a);
	}
}

void Router::subscribe(rsb::SubscriptionPtr s) {
	// notify ports about new subscription
	notifyPorts(s,rsb::filter::FilterAction::ADD);
	ep->subscribe(s);
}

void Router::unsubscribe(rsb::SubscriptionPtr s) {
	// notify ports about removal of subscription
	notifyPorts(s,rsb::filter::FilterAction::REMOVE);
	ep->unsubscribe(s);
}

}

}

