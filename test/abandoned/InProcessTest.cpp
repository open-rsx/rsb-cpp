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

#include "rsb/util/Task.h"
#include "rsb/util/PeriodicTask.h"
#include "rsb/transport/Port.h"
#include "rsb/transport/inprocess/InProcessPort.h"
#include "BTL2StringConverter.h"
#include "InformerTask.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>

using namespace std;
using namespace rsb;
using namespace rsb::test;
using namespace rsb::util;
using namespace rsb::transport;
using namespace rsb::inprocess;

int main(void) {
	// task execution service
	TaskExecutorVoidPtr exec(new TaskExecutor<void>());

	// in-process port
	PortPtr p(new InProcessPort());

	// domain objects
	boost::shared_ptr<InformerTask> source(new InformerTask(p));
	p->setObserver(boost::bind(&InformerTask::handler,source.get(),_1));

	// activate port and schedule informer
	p->activate();
	TaskPtr task_source = exec->schedulePeriodic<InformerTask>(source,0);

	// wait *here* for shutdown as this is not known to the Port
	{
		boost::recursive_mutex::scoped_lock lock(source->m);
		while (source->c!=10) {
			source->cond.wait(lock);
		}
	}

	// stop and join all threads
	exec->join(task_source); // no cancel needed as done already locally, see execute()
	// port is deactivated through dtr
	cerr << "InProcessTest finished" << endl;
}
