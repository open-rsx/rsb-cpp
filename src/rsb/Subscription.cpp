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

#include "Subscription.h"
#include <boost/bind.hpp>

using namespace std;

namespace rsb {

/**
 * Adapter from function-based Action to EventHandler.
 *
 * @author jwienke
 */
class ActionAdapter: public EventHandler {
private:

	Action a;

public:

	ActionAdapter(Action a) :
		a(a) {
	}

	void notify(EventPtr event) {
		a(event);
	}

};

Subscription::Subscription() :
	enabled(true) {
	filters = boost::shared_ptr<FilterChain>(new FilterChain());
	handlers = boost::shared_ptr<set<HandlerPtr> >(new set<HandlerPtr> ());
}

Subscription::~Subscription() {
}

void Subscription::appendFilter(rsb::filter::FilterPtr p) {
	filters->push_back(p);
}

void Subscription::appendHandler(HandlerPtr h) {
	handlers->insert(h);
}

void Subscription::appendAction(Action a) {
	handlers->insert(HandlerPtr(new ActionAdapter(a)));
}

bool Subscription::match(EventPtr e) {
	try {
		// call actions
		for (FilterChain::iterator f = filters->begin(); f != filters->end(); ++f) {
			if (!(*f)->match(e)) {
				return false;
			}
		}

	} catch (exception &e) {
		throw e;
	} catch (...) {
		// TODO better exception!!!
		throw runtime_error("unknown error during event matching");
	}
	return true;
}

boost::shared_ptr<FilterChain> Subscription::getFilters() {
	return filters;
}

boost::shared_ptr<std::set<HandlerPtr> > Subscription::getHandlers() {
	return handlers;
}

void Subscription::removeHandler(HandlerPtr h) {
	handlers->erase(h);
}

bool Subscription::isEnabled() {
	return enabled;
}

void Subscription::disable() {
	enabled = false;
}

ostream &operator<<(ostream &stream, const Subscription &subscription) {
	return stream << "Subscription[id = " << subscription.id << ", enabled = "
			<< subscription.enabled
			<< ", AND SEVERAL OTHER THINGS I DONT KNOW HOW TO PRINT RIGHT NOW]";
}

}
