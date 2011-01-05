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

#include "InformerTask.h"

#include "rsb/introspection/PortStateChange.h"

using namespace std;
using namespace rsb;
using namespace rsb::transport;
using namespace rsb::introspection;
using namespace rsc::threading;

namespace rsb {

namespace test {

InformerTask::InformerTask(PortPtr p, const unsigned int &numEvents) :
	numEvents(numEvents), sentEvents(0), port(p) {
}

InformerTask::~InformerTask() {
}

void InformerTask::execute() {
	++sentEvents;
	cout << sentEvents << " " << flush;
	PortStateChangePtr psc(new PortStateChange());
	psc->set_action("update");
	psc->set_comment("sent from informer task");
	psc->set_hostname("x200t");
	psc->set_message("port id is blub");
	psc->set_type("SpreadPort");
	string uri = "xcf://blah";
	if (sentEvents % 2 == 0) {
		// should be filtered by Port already
		uri = "xcf://blahblah";
	}
	RSBEventPtr p(new RSBEvent(uri, boost::static_pointer_cast<void>(psc),
			"portstatechange"));
	port->push(p);
	if (sentEvents == 2 * numEvents) {
		cout << endl;
		cancel();
	}
}

// ------

WaitingObserver::WaitingObserver(const unsigned int &desiredEvents) :
	desiredEvents(desiredEvents), receivedEvents(0) {

}

void WaitingObserver::handler(RSBEventPtr e) {
	boost::recursive_mutex::scoped_lock lock(m);
	++receivedEvents;
	cout << "Event #" << receivedEvents << "/" << desiredEvents
			<< " received. Metadata: " << *e << endl;
	PortStateChangePtr p = boost::static_pointer_cast<PortStateChange>(
			e->getData());
	cout << "Event #" << receivedEvents << "/" << desiredEvents
			<< " contains: " << p->action() << endl;
	if (receivedEvents == desiredEvents) {
		condition.notify_all();
	}
}

void WaitingObserver::waitReceived() {
	boost::recursive_mutex::scoped_lock lock(m);
	while (receivedEvents < desiredEvents) {
		condition.wait(lock);
	}
}

}
}
