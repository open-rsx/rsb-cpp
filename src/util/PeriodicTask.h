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

#ifndef PERIODICTASK_H_
#define PERIODICTASK_H_

#include "Task.h"

#include <iostream>

namespace rsb {

namespace util {

template < class R >
class PeriodicTask : public Task<R> {
public:
	PeriodicTask(boost::function<R(Task<R>*)> delegate, int ms) : Task<R>(delegate), cycleTime(ms), logger(log4cxx::Logger::getLogger("rsb.util.task")) {};
	virtual ~PeriodicTask() {
		LOG4CXX_TRACE(logger, "~PeriodicTask() entered");
	//	if (!cancelRequest) cancel();
	};

	virtual void cancel() {
		LOG4CXX_TRACE(logger, "PeriodicTask::cancel() entered");
		Task<R>::cancel();
	}

protected:
	virtual bool continueExec() {
		LOG4CXX_TRACE(logger, "~PeriodicTask()::continueExec() entered");
		// wait, give others a chance
		bool cont = false;
		if (cycleTime != 0) {
			boost::xtime time;
			xtime_get(&time, boost::TIME_UTC);
			time.nsec += cycleTime * 1000000;
			// TODO provide option to interrupt in cancel using boost::this_thread
			try {
				LOG4CXX_TRACE(logger, "PeriodicTask()::continueExec() before thread sleep, sleeping " << cycleTime << " ms");
				boost::thread::sleep(time);
			//	thread->sleep(time);
			} catch (boost::thread_interrupted e) {
				LOG4CXX_WARN(logger, "PeriodicTask()::continueExec() catched boost::thread_interrupted exception");
			}
			LOG4CXX_TRACE(logger, "PeriodicTask()::continueExec() thread woke up");
		}
		LOG4CXX_TRACE(logger, "PeriodicTask()::continueExec() before lock");
		//boost::recursive_mutex::scoped_lock lock(Task<R>::m);
		if (!this->cancelRequest) {
			cont = true;
		} else {
			LOG4CXX_TRACE(logger, "PeriodicTask()::continueExec() cancel requested");
		}
		LOG4CXX_TRACE(logger, "PeriodicTask()::continueExec() finished");
		return cont;
	}

private:
	int cycleTime;
	log4cxx::LoggerPtr logger;

};

}

}

#endif /* PERIODICTASK_H_ */
