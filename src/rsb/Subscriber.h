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

#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include "RSBEvent.h"
#include "Subscription.h"
#include "transport/Router.h"
#include "transport/Port.h"

#include <string>
#include <rsc/logging/Logger.h>
#include <boost/shared_ptr.hpp>

namespace rsb {

// TODO refactor commonalities of participants into a participant ?!? class
// TODO how to deal with IDs of subscribers? (it must be possible to reference them internally somehow
// TODO use templates in subscriptions only? (however, they need the event info)
class Subscriber {
public:
	Subscriber(std::string uri) :
		logger(rsc::logging::Logger::getLogger("rsb.Subscriber")), uri(uri), passive(false) {
		// TODO evaluate configuration
		router = transport::RouterPtr(new transport::Router(transport::TransportFactory::SPREAD,
				transport::TransportFactory::NONE));
		activate();
	}

	Subscriber(transport::TransportFactory::PortTypes in, std::string uri) :
		logger(rsc::logging::Logger::getLogger("rsb.Subscriber")), uri(uri), passive(false) {
		// TODO evaluate configuration
		router = transport::RouterPtr(new transport::Router(in, transport::TransportFactory::NONE));
		activate();
	}

	virtual ~Subscriber() {
		if (!passive) deactivate();
	};

//	static SubscriberPtr create(std::string uri) {
//		boost::shared_ptr<Subscriber> s(new Subscriber());
//		s->activate();
//		//s->setShared(s);
//	}

	void activate() {
		router->activate();
		passive = false;
	}

	void deactivate() {
		if (!passive) router->deactivate();
		passive = true;
	}

	void addSubscription(SubscriptionPtr s) {
		router->subscribe(s);
	}

	void removeSubscription(SubscriptionPtr s) {
		router->unsubscribe(s);
	}

protected:
	Subscriber() { /* forbidden */ };

private:
	rsc::logging::LoggerPtr logger;
	std::string uri;
	volatile bool passive;
	rsb::transport::RouterPtr router;

};

typedef boost::shared_ptr<Subscriber> SubscriberPtr;

}

#endif /* SUBSCRIBER_H_ */
