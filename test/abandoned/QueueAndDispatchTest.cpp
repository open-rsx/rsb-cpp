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

#include "rsc/util/Task.h"
#include "rsc/util/TaskExecutor.h"

#include "rsb/RSBEvent.h"
#include "rsb/transport/QueueAndDispatchTask.h"
#include "BTL2StringConverter.h"

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>

using namespace std;
using namespace rsb;
using namespace rsb::util;
using namespace rsb::transport;

class NotificationTask {
public:
	NotificationTask(boost::shared_ptr<QueueAndDispatchTask<RSBEventPtr> > receiver) : c(0), i(0), r(receiver) {};

	int c;
	boost::recursive_mutex m;
	boost::condition cond;

	void execute(Task<void>* t) {
		i++;
		cout << i << " " << flush;
		PersonPtr p1(new Person());
		RSBEventPtr p(new RSBEvent("xcf://blah",boost::static_pointer_cast<void>(p1),"Person"));
		r->addElement(p);
		if (i==10) {
			cout << endl;
			t->cancel();
		}
	}

	void handler(RSBEventPtr e) {
		c++;
		cout << "Event #" << c << "/10 received: URI = " << e->getURI() << endl << flush;
		//if (c==10) r->cancel();
		if (c==10) {
			boost::recursive_mutex::scoped_lock lock(m);
			cond.notify_all();
		}
	}
private:
	int i;
	boost::shared_ptr<QueueAndDispatchTask<RSBEventPtr > > r;

};

int main(void) {
	// task execution service
	TaskExecutorVoidPtr exec(new TaskExecutor<void>());

	// domain objects
	boost::shared_ptr<QueueAndDispatchTask< RSBEventPtr > > sink(new QueueAndDispatchTask<RSBEventPtr>());
	boost::shared_ptr<NotificationTask> source(new NotificationTask(sink));
	sink->setObserver(boost::bind(&NotificationTask::handler,source.get(),_1));

	// schedule the two tasks
	TaskPtr task_source = exec->schedulePeriodic<NotificationTask>(source,0);
	TaskPtr task_sink = exec->schedulePeriodic<QueueAndDispatchTask< RSBEventPtr > >(sink,0);

	// wait *here* for shutdown as this is not known to the QueueAndDispatchTask
	{
		boost::recursive_mutex::scoped_lock lock(source->m);
		while (source->c!=10) {
			source->cond.wait(lock);
		}
	}

	// stop and join all threads
	exec->join(task_source); // no cancel needed as done already locally, see execute()
	sink->cancel();
	exec->join(task_sink);
	return EXIT_SUCCESS;
}
