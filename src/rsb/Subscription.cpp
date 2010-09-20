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

class ActionAdapter: public EventHandler {
private:

	Action a;

public:

	ActionAdapter(Action a) :
		a(a) {
	}

	void notify(RSBEventPtr event) {
		a(event);
	}

};

Subscription::Subscription() :
	enabled(true) {
	filters = boost::shared_ptr<FilterChain>(new FilterChain());
	handlers = boost::shared_ptr<set<HandlerPtr> >(new set<HandlerPtr> ());
}

Subscription::~Subscription() {
	// TODO Auto-generated destructor stub
}

void Subscription::appendFilter(rsb::filter::AbstractFilterPtr p) {
	filters->push_back(p);
}

void Subscription::appendHandler(HandlerPtr h) {
	handlers->insert(h);
}

void Subscription::appendAction(Action a) {
	handlers->insert(HandlerPtr(new ActionAdapter(a)));
}

bool Subscription::match(RSBEventPtr e) {
	try {
		// call actions
		for (FilterChain::iterator f = filters->begin(); f != filters->end(); ++f) {
			if (!(*f)->match(e)) {
				return false;
			}
		}

	} catch (...) {
		throw "unknown error during event matching";
	}
	return true;
}

}
