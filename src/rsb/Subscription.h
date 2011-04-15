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

#include <list>
#include <map>
#include <set>
#include <boost/function.hpp>

#include "Event.h"
#include "Handler.h"
#include "filter/Filter.h"
#include "rsb/rsbexports.h"

namespace rsb {

typedef std::list<rsb::filter::FilterPtr> FilterChain;

/**
 * Each Subscription object is associated to a @ref
 * rsb::Listener. Appending a @ref rsb::filter::Filter to the
 * subscription restricts received events to those matching the new
 * filter in addition to all already installed filters.
 *
 * The following example shows using a Subscription using a @ref
 * rsb::filter::ScopeFilter (assume the same uri is used as for the
 * Listener) and a @ref rsb::QueuePushHandler as a @ref rsb::Handler
 * which pushes the received events to a given queue:
 *
 * @todo update this
 * @code
 * Subscription subscription = new Subscription();
 * FilterPtr f(new ScopeFilter("rsb://example/informer"));
 * subscription->appendFilter(f);
 * boost::shared_ptr<rsc::SynchronizedQueue<string> > myQueue(new rsc::SynchronizedQueue<string>);
 * boost::shared_ptr<rsb::QueuePushHandler<string> > qph(new QueuePushHandler<string> (myQueue));
 * subscription->appendHandler(qph);
 * @endcode
 *
 * @todo make actions and filter thread-safe, especially for match and action
 *       execution which may be asynchronous to changes in theses subscriptions
 *       I think using a list in a shared_ptr is the wrong way to achieve this.
 */
class RSB_EXPORT Subscription {
public:

	Subscription();
	virtual ~Subscription();

	/**
	 * Appends a Filter to the Subscription. Every event not matching the
	 * constraints provided by the given filter is ignored in processing by the
	 * Handlers.
	 *
	 * @param p a Pointer to the Filter.
	 */
	virtual void appendFilter(rsb::filter::FilterPtr p);

	virtual bool match(EventPtr e);

	/**
	 * @todo check if it is generally better to return iterators?!
	 */
	boost::shared_ptr<FilterChain> getFilters();

	bool isEnabled();

	void disable();

	friend RSB_EXPORT std::ostream &operator<<(std::ostream &stream,
			const Subscription &subscription);

private:
	volatile bool enabled;
	rsc::misc::UUID id;
	boost::shared_ptr<FilterChain> filters;
};

typedef boost::shared_ptr<Subscription> SubscriptionPtr;

/**
 * @todo really implement stream operators
 */
RSB_EXPORT std::ostream
&operator<<(std::ostream &stream, const Subscription &subscription);

}
