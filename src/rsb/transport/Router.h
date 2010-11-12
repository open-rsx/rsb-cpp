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

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>

#include "TransportFactory.h"
#include "../filter/FilterObservable.h"
#include "../EventProcessor.h"
#include "../Subscription.h"
#include "../QualityOfServiceSpec.h"

namespace rsb {
namespace transport {

/**
 *
 *
 * @author swrede
 * @todo extend incoming and outgoing ports to sets of ports
 * @todo add configuration, provide preliminary set up interface
 * @todo implement abstract factory pattern for different port types
 * @todo think about null objects for ports to avoid checks for existence
 */
class Router { //: //public rsb::filter::FilterObservable {
public:
	Router(TransportFactory::PortTypes inType = TransportFactory::LOCAL,
			TransportFactory::PortTypes outType = TransportFactory::LOCAL);
	virtual ~Router();

	void activate();
	void deactivate();

	/**
	 * Publish event to out ports of this router.
	 *
	 * @param e event to publish
	 */
	void publish(RSBEventPtr e);

	/**
	 * Add a subscription.
	 *
	 * @param s subscription to add
	 */
	void subscribe(rsb::SubscriptionPtr s);

	/**
	 * Unsubscribe a subscription.
	 *
	 * @param s subscription to remove
	 */
	void unsubscribe(rsb::SubscriptionPtr s);

	/**
	 * Define the desired quality of service specifications for published
	 * events.
	 *
	 * @param specs QoS specification
	 * @throw UnsupportedQualityOfServiceException requirements cannot be met
	 */
	void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);

	// HACK for current tests
	// TODO remove header implementation
	PortPtr getOutPort() {
		return outPort;
	}
	PortPtr getInPort() {
		return inPort;
	}

protected:
	/**
	 * Helper for port notification about subscription status changes.
	 */
	void
	notifyPorts(rsb::SubscriptionPtr s, rsb::filter::FilterAction::Types a);

private:
	rsc::logging::LoggerPtr logger;
	PortPtr inPort;
	PortPtr outPort;
	// ep for observation model
	rsb::internal::EventProcessorPtr eventProcessor;
	volatile bool shutdown;

};

typedef boost::shared_ptr<Router> RouterPtr;

}
}

#endif /* ROUTER_H_ */
