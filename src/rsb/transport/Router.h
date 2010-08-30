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

#ifndef ROUTER_H_
#define ROUTER_H_

#include "TransportFactory.h"
#include "../filter/FilterObservable.h"
#include "../EventProcessor.h"
#include "../Subscription.h"

#include <rsc/logging/Logger.h>
#include <boost/shared_ptr.hpp>

namespace rsb {

namespace transport {

// TODO extend incoming and outgoing ports to sets of ports
// TODO add configuration, provide preliminary set up interface
// TODO implement abstract factory pattern for different port types
class Router { //: //public rsb::filter::FilterObservable {
public:
	Router(TransportFactory::PortTypes inType = TransportFactory::LOCAL, TransportFactory::PortTypes outType = TransportFactory::LOCAL);
	virtual ~Router();

	void activate();
	void deactivate();

	// publish event to out ports of this router
	void publish(RSBEventPtr e);

    // add a subscription
    void subscribe(rsb::SubscriptionPtr s);

    // unsubscribe a subscription
    void unsubscribe(rsb::SubscriptionPtr s);

    // HACK for current tests
    PortPtr getOutPort() {
    	return op;
    }
    PortPtr getInPort() {
    	return ip;
    }

protected:
    // helpfer for port notification about subscription status changes
    void notifyPorts(rsb::SubscriptionPtr s, rsb::filter::FilterAction::Types a);

private:
	rsc::logging::LoggerPtr logger;
	PortPtr ip, op;
	// ep for observation model
	rsb::internal::EventProcessorPtr ep;
	volatile bool shutdown;
};

typedef boost::shared_ptr<Router> RouterPtr;

}

}

#endif /* ROUTER_H_ */
