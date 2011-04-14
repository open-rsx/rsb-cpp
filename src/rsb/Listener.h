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

#pragma once

#include <string>

#include <rsc/logging/Logger.h>

#include <boost/shared_ptr.hpp>

#include "Event.h"
#include "Subscription.h"
#include "eventprocessing/Router.h"
#include "transport/Connector.h"
#include "rsb/rsbexports.h"

namespace rsb {

/**
 * A Listener subscribes to published events by a @ref rsb::Informer by maintaining
 * several @ref rsb::Subscription instances. Subscriptions must at least provide a @ref rsb::filter::ScopeFilter
 * to inform the listener about the desired events to receive.
 *
 * Usage example:
 * @code
 * ListenerPtr s(new Listener("blub"));
 * SubscriptionPtr sub(new Subscription());
 * FilterPtr f(new ScopeFilter("rsb://example/informer"));
 * sub->appendFilter(f);
 * boost::shared_ptr<MyDataHandler> dh(new MyDataHandler());
 * sub->appendHandler(dh);
 * s->addSubscription(sub);
 * @endcode
 *
 * @author swrede
 *
 * @todo refactor commonalities of participants into a participant ?!? class
 * @todo how to deal with IDs of listeners? (it must be possible to reference them internally somehow
 * @todo use templates in subscriptions only? (however, they need the event info)
 */
class RSB_EXPORT Listener : public Participant {
public:

	/**
	 * Constructs a new Listener assigned to the specified uri. The Listener
	 * opens a @ref rsb::eventprocessing::Router to ConnectorType Spread and is activated after construction.
	 *
	 * @param uri the uri where the data is published.
	 */
	Listener(const std::string &uri);

	/**
	 * Constructs a new Listener assigned to the specified uri. The Listener
	 * opens a @ref rsb::eventprocessing::Router to a ConnectorType specified and is activated after
	 * construction.
	 *
	 * @param uri the uri where the data is published.
	 * @param in the ConnectorType (enum) to connect with, i.e.
	 *           transport::Factory::SPREAD
	 */
	Listener(transport::Factory::ConnectorTypes in, const std::string &uri);

	virtual ~Listener();

	/**
	 * Activates the Listener and therefore the Router. Is considered being in
	 * active mode afterwards.
	 */
	void activate();

	/**
	 * Deactivates the Listener and therefore the Router. Is considered being
	 * in passive mode afterwards.
	 */
	void deactivate();

	/**
	 * Adds a Subscription to the Listener.
	 *
	 * @param s a Pointer to the Subscription added.
	 */
	void addSubscription(SubscriptionPtr s);

	/**
	 * Removes a Subscription to the Listener.
	 *
	 * @param s a Pointer to the Subscription removed.
	 */
	void removeSubscription(SubscriptionPtr s);

protected:

	/**
	 * Forbidden constructor.
	 */
	Listener();

private:
	rsc::logging::LoggerPtr logger;
	std::string uri;
	volatile bool passive;
	eventprocessing::RouterPtr router;

};

typedef boost::shared_ptr<Listener> ListenerPtr;

template <typename Ch,
	  typename Tr>
std::basic_ostream<Ch, Tr>&
operator<<(std::basic_ostream<Ch, Tr>& stream,
	   const Listener&             listener) {
	stream << "Listener[id=" << listener.getUUID() << "]";
	return stream;
}

}