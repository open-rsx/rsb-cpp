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

#ifndef TASKEXECUTOR_H_
#define TASKEXECUTOR_H_

#include "Task.h"
#include "PeriodicTask.h"

#include <map>
#include <boost/thread.hpp>

namespace rsb {

namespace util {

typedef boost::shared_ptr< boost::thread > TaskThreadPtr;
typedef std::map < TaskPtr, TaskThreadPtr > TaskMap;
// TODO refactor to use boost::ptr_map
typedef boost::shared_ptr< TaskMap > TaskMapPtr;

template < class R >
class TaskExecutor {
public:
	TaskExecutor() {
		tasks = TaskMapPtr(new std::map < boost::shared_ptr< Task< R > >, TaskThreadPtr >());
	}
	virtual ~TaskExecutor() {};

	template < class T >
	TaskPtr createTask(T* callable, bool periodic = false, int freq = 0) {
		TaskPtr t;
		if (periodic) {
			t = TaskPtr(new PeriodicTask<void> (boost::bind(&T::execute, callable, _1),freq));
		} else {
			t = TaskPtr(new Task<void> (boost::bind(&T::execute, callable, _1)));
		}
		return t;
	}

	template < class T >
	TaskPtr createTask(boost::shared_ptr<T> p, bool periodic = false, int freq = 0) {
		TaskPtr t;
		if (periodic) {
			t = TaskPtr(new PeriodicTask<void> (boost::bind(&T::execute, p.get(), _1),freq));
		} else {
			t = TaskPtr(new Task<void> (boost::bind(&T::execute, p.get(), _1)));
		}
		return t;
	}

	// TODO generalize to different task types, i.e. with different return types
	// TODO Possibly return a future, probably split task creation and scheduling
	template < class T >
	TaskPtr schedule(T* callable) {
		TaskPtr t = createTask(callable);
		//TaskPtr t = TaskPtr(new Task<void> (boost::bind(&T::execute, callable, _1)));
		schedule(t);
		//(*tasks)[t] = t->start();
		return t;
	}

	template < class T >
	TaskPtr schedule(T* callable, int freq) {
		TaskPtr t = createTask(callable,true,freq);
		// TaskPtr t = TaskPtr(new PeriodicTask<void> (boost::bind(&T::execute, callable, _1),freq));
		schedule(t);
		//(*tasks)[t] = t->start();
		return t;
	}

	template < class T >
	TaskPtr schedule(boost::shared_ptr<T> p) {
		TaskPtr t = createTask(p);
		//TaskPtr t = TaskPtr(new Task<void> (boost::bind(&T::execute, p.get(),_1)));
		schedule(t);
		return t;
	}

	template < class T >
	TaskPtr schedulePeriodic(boost::shared_ptr<T> p, int freq) {
		TaskPtr t = createTask(p,true,freq);
		// TaskPtr t = TaskPtr(new PeriodicTask<void> (boost::bind(&T::execute, p.get(),_1),freq));
		//(*tasks)[t] = t->start();
		schedule(t);
		return t;
	}

	void schedule(TaskPtr t) {
		(*tasks)[t] = t->start();
	}

	void join(TaskPtr tp) {
		TaskThreadPtr t = (*tasks)[tp];
		if (t) {
			// TODO check exact semantics of joinable
			if (t->joinable()) t->join();
//	    TaskMap::iterator i = find(tasks.begin(), tasks.end(), tp);
//	    if(i != tasks.end()) {
//			boost::thread t = i->second;
//			// TODO check exact semantics of joinable
//			if (t.joinable()) t->join();
			tasks->erase(tp);
	    } else {
	    	// ignore
	    	std::cerr << "WARN: task " << tp << " not found" << std::endl;
	    }
	}

private:
	TaskMapPtr tasks;
};

typedef boost::shared_ptr< TaskExecutor < void > > TaskExecutorVoidPtr;

}

}

#endif /* TASKEXECUTOR_H_ */
