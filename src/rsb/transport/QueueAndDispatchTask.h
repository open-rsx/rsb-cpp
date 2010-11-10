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

#ifndef QUEUEANDDISPATCHTASK_H_
#define QUEUEANDDISPATCHTASK_H_

#include "../util/Task.h"
#include "../RSBEvent.h"

#include <list>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/function.hpp>

namespace rsb {

namespace transport {

template<class T>
class QueueAndDispatchTask {
public:
	QueueAndDispatchTask() :
		cancelRequested(false) {
		queue = boost::shared_ptr<std::list<T> >(new std::list<T>());
	}
	virtual ~QueueAndDispatchTask() {
		//std::cerr << "~QueueAndDispatchTask()" << std::endl;
	}
	;

	void addElement(T e) {
		{
			boost::recursive_mutex::scoped_lock lock(m);
			queue->push_back(e);
		}
		c.notify_all();
	}

	void setObserver(boost::function<void(T)> act) {
		observer = act;
	}

	void cancel() {
		{
			// protect setting and comparison of cancel boolean, see execute()
			boost::recursive_mutex::scoped_lock lock(m);
			cancelRequested = true;
		}
		c.notify_all();
	}

	void execute(rsb::util::Task<void>* t) {
		T e;
		boost::recursive_mutex::scoped_lock lock(m);
		// spurious unblocking loop
		while (queue->empty() && !cancelRequested) {
			// stop thread and wait for next event
			//std::cout << "Condition locked!!!" << std::endl;
			c.wait(lock);
			//std::cout << "Condition unlocked!!!" << std::endl;
		}

		if (cancelRequested) {
			lock.unlock();
			t->cancel();
			return;
		}

		assert(!queue->empty());

		e = queue->front();
		queue->pop_front();

		// dispatch without lock
		lock.unlock();
		if (observer) {
			observer(e);
		}
	}

private:
	boost::shared_ptr<std::list<rsb::RSBEventPtr> > queue;
	boost::function<void(T)> observer;

	mutable boost::recursive_mutex m;
	boost::condition c;

	volatile bool cancelRequested;

};

}

}

#endif /* QUEUEANDDISPATCHTASK_H_ */
