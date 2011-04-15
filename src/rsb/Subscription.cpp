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

using namespace std;

namespace rsb {

Subscription::Subscription() :
	enabled(true) {
	filters = boost::shared_ptr<FilterChain>(new FilterChain());
}

Subscription::~Subscription() {
}

void Subscription::appendFilter(rsb::filter::FilterPtr p) {
        filters->push_back(p);
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
