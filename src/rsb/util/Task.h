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

#ifndef TASK_H_
#define TASK_H_

#include "Timer.h"

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <rsc/logging/Logger.h>

namespace rsb {

namespace util {

template < class R >
class Task;
typedef boost::shared_ptr<Task<void> > TaskPtr;

//template < class R >
//typedef boost::shared_ptr<Task<R> > TaskPtr;
// TODO think about if its possible to pass a smart pointer to this object
//      to the callbacks
typedef boost::function<void()> Callback;

template < class R >
class Task {
public:
	Task(boost::function<R(Task<R>*)> delegate) : logger(rsc::logging::Logger::getLogger("rsb.util.Task")) {
		d = delegate;
		cancelRequest = false;
		cancelled = false;
		// TODO add id
	    timer = TimerPtr(new Timer("TimedObject"));
	    timer->start();
	//    pre = boost::bind(&TimedObject::beforeCycle,this);
	//    post = boost::bind(&TimedObject::afterCycle,this);
	}

	virtual ~Task() {
		// TODO Auto-generated destructor stub
	}

	void run() {
		RSBTRACE(logger, "run() entered"); // << *id);
		// TODO Think about returning an iterator to the results of execute here!
		do {
			// TODO add exception handling
			// call pre hook
			if (pre)
				pre();
			// call template method
			d(this);
			// call post hook
			if (post)
				post();
			RSBTRACE(logger, "run cycle done");
		} while (continueExec());
		boost::recursive_mutex::scoped_lock lock(m);
		RSBINFO(logger, "run() finished");
		cancelled = true;
		this->c.notify_all();
	}

	boost::shared_ptr<boost::thread> start() {
		boost::shared_ptr<boost::thread> thread = boost::shared_ptr<boost::thread>(new boost::thread(
				boost::bind(&Task::run, this)));
		return thread;
	}

	// beautify this
	// provide time mesaurement as default implementation
	void setPreCallback(Callback f) {;
		pre = f;
	}

	void setPostCallback(Callback f) {;
		pre = f;
	}

	virtual void cancel() {
		RSBTRACE(logger, "Task::cancel() entered");
//		// protect setting and comparison of cancel boolean, see execute()
//		boost::recursive_mutex::scoped_lock lock(m);
		cancelRequest = true;
	}

	virtual bool isCancelRequested() {
		return cancelRequest;
	}
	virtual void restart() {
		cancelRequest = false;
	}

	virtual void waitDone() {
		RSBDEBUG(logger, "waitDone() entered");
		boost::recursive_mutex::scoped_lock lock(m);
		RSBDEBUG(logger, "waitDone() after lock, before wait");
		while (!cancelled) {
			this->c.wait(lock);
		}
		RSBDEBUG(logger, "waitDone() finished");
	}


protected:

	boost::function<R(Task<R>*)> d;
	//boost::shared_ptr<boost::thread> thread;

	mutable boost::recursive_mutex m;
    boost::condition c;

	//void TimedObject::beforeCycle(TimedObjectPtr p) {
	void beforeCycle() {
		// store last time
		timestamp = timer->getElapsed();
	}

	void afterCycle() {
		// calculate processing time for last cycle, last n cycle, variance...
		// TODO change to logging
		RSBINFO(logger, "Times (last cycle = " << timer->getElapsed()-timestamp << "ms)");
	}

	virtual bool continueExec() {
		return false;
	};

	volatile bool cancelRequest;
	volatile bool cancelled;

private:
	rsc::logging::LoggerPtr logger;
	double timestamp;
	TimerPtr timer;
	Callback pre;
	Callback post;

};

}

}

#endif /* TASK_H_ */
