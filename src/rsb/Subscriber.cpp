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

#include "Subscriber.h"

using namespace std;

namespace rsb {

Subscriber::Subscriber(const string &uri) :
	logger(rsc::logging::Logger::getLogger("rsb.Subscriber")), uri(uri),
			passive(false) {
	// TODO evaluate configuration
	router = transport::RouterPtr(
			new transport::Router(transport::TransportFactory::SPREAD,
					transport::TransportFactory::NONE));
	activate();
}

Subscriber::Subscriber(transport::TransportFactory::PortTypes in,
		const string &uri) :
	logger(rsc::logging::Logger::getLogger("rsb.Subscriber")), uri(uri),
			passive(false) {
	// TODO evaluate configuration
	router = transport::RouterPtr(
			new transport::Router(in, transport::TransportFactory::NONE));
	activate();
}

Subscriber::~Subscriber() {
	if (!passive) {
		deactivate();
	}
}

void Subscriber::activate() {
	router->activate();
	passive = false;
}

void Subscriber::deactivate() {
	if (!passive) {
		router->deactivate();
	}
	passive = true;
}

void Subscriber::addSubscription(SubscriptionPtr s) {
	router->subscribe(s);
}

void Subscriber::removeSubscription(SubscriptionPtr s) {
	router->unsubscribe(s);
}

Subscriber::Subscriber() {
}

}
