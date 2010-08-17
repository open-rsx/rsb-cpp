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

namespace rsb {

Subscription::Subscription() : enabled(true) {
	filters = boost::shared_ptr<FilterChain>(new FilterChain());
	actions = boost::shared_ptr<Actions>(new Actions());
}

Subscription::~Subscription() {
	// TODO Auto-generated destructor stub
}

void Subscription::appendFilter(rsb::filter::AbstractFilterPtr p) {
	filters->push_back(p);
}

void Subscription::appendAction(Action a) {
	actions->push_back(a);
}

bool Subscription::match(RSBEventPtr e) {
	bool match = true;
	try {
		// call actions
		for (FilterChain::iterator f = filters->begin(); f!= filters->end(); ++f) {
			if (!(*f)->match(e)) {
				match = false;
				continue;
			}
		}

	} catch (...) {
		throw "unknown error during event matching";
	}
	return match;
}

}
