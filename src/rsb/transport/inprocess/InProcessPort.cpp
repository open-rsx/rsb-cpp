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

#include "InProcessPort.h"
#include "../../util/Configuration.h"
#include "../../CommException.h"

#include <rsc/logging/Logger.h>
#include <iostream>
#include <string.h>

#include "../AbstractConverter.h"

#include <boost/bind.hpp>

using namespace std;
using namespace rsc::logging;
using namespace rsb;
using namespace rsb::util;
using namespace rsb::transport;

namespace {
LoggerPtr logger(Logger::getLogger("rsb.inprocess"));
}

namespace rsb {

namespace inprocess {

InProcessPort::InProcessPort(){
	shutdown = false;
	exec = TaskExecutorVoidPtr(new TaskExecutor<void>());

	// TODO check if it makes sense and is possible to provide a weak_ptr to the ctr of StatusTask
	st = boost::shared_ptr<StatusTask>(new StatusTask(this));
	qad = boost::shared_ptr<QueueAndDispatchTask<RSBEventPtr > >(new QueueAndDispatchTask<RSBEventPtr>());
}

InProcessPort::~InProcessPort() {
	deactivate();
	cout << "InProcessPort::~InProcessPort()" << endl;
}

void InProcessPort::activate() {
	// (re-)start threads
	qadTask = exec->schedulePeriodic<QueueAndDispatchTask<RSBEventPtr> >(qad,0);
	staTask = exec->schedulePeriodic<StatusTask>(st,500);
}

void InProcessPort::deactivate() {
	shutdown = true;
	cout << "stopping qad task object" << endl;
	qad->cancel();
	cout << "stopping qad thread" << endl;
	exec->join(qadTask);
	cout << "stopping st task" << endl;
	staTask->cancel();
	cout << "stopping st thread" << endl;
	exec->join(staTask);
	cout << "InProcessPort::deactivate finished" << endl;
};

void InProcessPort::push(RSBEventPtr e) {
	// get matching converter -- can be skipped here
	//	string s;
	//	boost::shared_ptr<void> obj = boost::static_pointer_cast<void>(e->getData());
	//	boost::shared_ptr<AbstractConverter<string> > c = boost::static_pointer_cast<AbstractConverter<string> >((*converters)["person"]);
	//	c->serialize("person",obj,s);
	//	cout << "Result of serialize1 call: " << s << endl;

	// if (only_id) subscription just call listener directly?!?
	// don't dispatch via matching subsystem?!? otherwise mark at least event for IdFilter as matched
	// localize dispatching in matching subsystem, don't make these ports here to complicated...
	// decide this based on considerations of pattern implementation / configuration examples

	// put new element in queue
	qad->addElement(e);
}

void InProcessPort::setObserver(Action a) {
	qad->setObserver(a);
}

void InProcessPort::removeObserver(Action a) {
	qad->setObserver(NULL);
}

}

}
