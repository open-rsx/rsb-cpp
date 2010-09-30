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

/**
 * A Subscriber subscribes to published events by a @ref rsb::Publisher by maintaining
 * several @ref rsb::Subscription instances. Subscriptions must at least provide a @ref rsb::filter::ScopeFilter
 * to inform the subscriber about the desired events to receive.
 *
 * Usage example:
 * @code
 * SubscriberPtr s(new Subscriber("blub"));
 * SubscriptionPtr sub(new Subscription());
 * AbstractFilterPtr f(new ScopeFilter("rsb://example/informer"));
 * sub->appendFilter(f);
 * boost::shared_ptr<MyDataHandler> dh(new MyDataHandler());
 * sub->appendHandler(dh);
 * s->addSubscription(sub);
 * @endcode
 *
 * @author swrede
 *
 * @todo refactor commonalities of participants into a participant ?!? class
 * @todo how to deal with IDs of subscribers? (it must be possible to reference them internally somehow
 * @todo use templates in subscriptions only? (however, they need the event info)
 * @todo remove header implementations
 */
class Subscriber {
public:

	/**
	 * Constructs a new Subscriber assigned to the specified uri. The Subscriber
	 * opens a @ref rsb::transport::Router to PortType Spread and is activated after construction.
	 *
	 * @param uri the uri where the data is published.
	 */
	Subscriber(std::string uri) :
		logger(rsc::logging::Logger::getLogger("rsb.Subscriber")), uri(uri),
				passive(false) {
		// TODO evaluate configuration
		router = transport::RouterPtr(new transport::Router(
				transport::TransportFactory::SPREAD,
				transport::TransportFactory::NONE));
		activate();
	}

	/**
	 * Constructs a new Subscriber assigned to the specified uri. The Subscriber
	 * opens a @ref rsb::transport::Router to a PortType specified and is activated after
	 * construction.
	 *
	 * @param uri the uri where the data is published.
	 * @param in the PortType (enum) to connect with, i.e.
	 *           transport::TransportFactory::SPREAD
	 */
	Subscriber(transport::TransportFactory::PortTypes in, std::string uri) :
		logger(rsc::logging::Logger::getLogger("rsb.Subscriber")), uri(uri),
				passive(false) {
		// TODO evaluate configuration
		router = transport::RouterPtr(new transport::Router(in,
				transport::TransportFactory::NONE));
		activate();
	}

	virtual ~Subscriber() {
		if (!passive)
			deactivate();
	}

	//	static SubscriberPtr create(std::string uri) {
	//		boost::shared_ptr<Subscriber> s(new Subscriber());
	//		s->activate();
	//		//s->setShared(s);
	//	}


	/**
	 * Activates the Subscriber and therefore the Router. Is considered being in
	 * active mode afterwards.
	 */
	void activate() {
		router->activate();
		passive = false;
	}

	/**
	 * Deactivates the Subscriber and therefore the Router. Is considered being
	 * in passive mode afterwards.
	 */
	void deactivate() {
		if (!passive)
			router->deactivate();
		passive = true;
	}

	/**
	 * Adds a Subscription to the Subscriber.
	 *
	 * @param s a Pointer to the Subscription added.
	 */
	void addSubscription(SubscriptionPtr s) {
		router->subscribe(s);
	}

	/**
	 * Removes a Subscription to the Subscriber.
	 *
	 * @param s a Pointer to the Subscription removed.
	 */
	void removeSubscription(SubscriptionPtr s) {
		router->unsubscribe(s);
	}

protected:
	Subscriber() { /* forbidden */
	}

private:
	rsc::logging::LoggerPtr logger;
	std::string uri;
	volatile bool passive;
	rsb::transport::RouterPtr router;

};

typedef boost::shared_ptr<Subscriber> SubscriberPtr;

}

#endif /* SUBSCRIBER_H_ */
