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
using namespace rsb::util;
using namespace rsb::transport;
using namespace rsb::introspection;

namespace rsb {

namespace test {

InformerTask::InformerTask(PortPtr p) : c(0), i(0), port(p) {
	// blank
};

InformerTask::~InformerTask() {
	// TODO Auto-generated destructor stub
}

void InformerTask::execute(Task<void>* t) {
	i++;
	cout << i << " " << flush;
	PortStateChangePtr psc(new PortStateChange());
	psc->set_action("update");
	psc->set_comment("sent from informer task");
	psc->set_hostname("x200t");
	psc->set_message("port id is blub");
	psc->set_type("SpreadPort");
	string uri = "xcf://blah";
	if (i%2==0) {
		// should be filtered by SpreadPort already
		uri = "xcf://blahblah";
	}
	RSBEventPtr p(new RSBEvent(uri, boost::static_pointer_cast<void>(
			psc), "portstatechange"));
	port->push(p);
	if (i == 20) {
		cout << endl;
		t->cancel();
	}
}

void InformerTask::handler(RSBEventPtr e) {
	c++;
	cout << "Event #" << c << "/10 received. Metadata: " << *e << endl;
	PortStateChangePtr p = boost::static_pointer_cast<PortStateChange>(e->getData());
	cout << "Event #" << c << "/10 contains: " << p->action() << endl;
	if (c == 10) {
		boost::recursive_mutex::scoped_lock lock(m);
		cond.notify_all();
	}
}

}

}
