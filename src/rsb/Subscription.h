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

#ifndef SUBSCRIPTION_H_
#define SUBSCRIPTION_H_

#include "RSBEvent.h"
#include "Handler.h"
#include "filter/AbstractFilter.h"

#include <list>
#include <map>
#include <set>
#include <boost/function.hpp>

namespace rsb {

typedef boost::function<void(RSBEventPtr)> Action;
// Q: Does it make sense to wrap boost::function in a shared_ptr?

typedef std::list<rsb::filter::AbstractFilterPtr> FilterChain;
typedef std::list<Action> Actions;

// TODO implement stream operators
// TODO add id field

/**
 * @todo remove header implementations
 * @todo make actions and filter thread-safe, especially for match and action
 *       execution which may be asynchronous to changes in theses subscriptions
 *       I think using a list in a shared_ptr is the wrong way to achieve this.
 */
class Subscription {
public:
	Subscription();
	virtual ~Subscription();

	virtual void appendFilter(rsb::filter::AbstractFilterPtr p);

	virtual bool match(RSBEventPtr e);

	virtual void appendAction(Action a);
	virtual void appendHandler(HandlerPtr h);

	// TODO check if it is generally better to return iterators?!
	boost::shared_ptr<FilterChain> getFilters() {
		return filters;
	}

	boost::shared_ptr<std::set<HandlerPtr> > getHandlers() {
		return handlers;
	}

	void removeHandler(HandlerPtr h) {
		handlers->erase(h);
	}

	bool isEnabled() {
		return enabled;
	}

	void disable() {
		enabled = false;
	}

private:
	volatile bool enabled;
	boost::shared_ptr<FilterChain> filters;
	boost::shared_ptr<std::set<HandlerPtr> > handlers;

};

typedef boost::shared_ptr<Subscription> SubscriptionPtr;

}

#endif /* SUBSCRIPTION_H_ */
