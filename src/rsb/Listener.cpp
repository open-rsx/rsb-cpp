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

#include "Listener.h"

using namespace std;

namespace rsb {

Listener::Listener(const string &uri) :
	logger(rsc::logging::Logger::getLogger("rsb.Listener")), uri(uri), passive(
			false) {
	// TODO evaluate configuration
	router = transport::RouterPtr(new transport::Router(
			transport::Factory::SPREAD,
			transport::Factory::NONE));
	activate();
}

Listener::Listener(transport::Factory::PortTypes in, const string &uri) :
	logger(rsc::logging::Logger::getLogger("rsb.Listener")), uri(uri), passive(
			false) {
	// TODO evaluate configuration
	router = transport::RouterPtr(new transport::Router(in,
			transport::Factory::NONE));
	activate();
}

Listener::~Listener() {
	if (!passive) {
		deactivate();
	}
}

void Listener::activate() {
	router->activate();
	passive = false;
}

void Listener::deactivate() {
	if (!passive) {
		router->deactivate();
	}
	passive = true;
}

void Listener::addSubscription(SubscriptionPtr s) {
	router->subscribe(s);
}

void Listener::removeSubscription(SubscriptionPtr s) {
	router->unsubscribe(s);
}

Listener::Listener() {
}

}
