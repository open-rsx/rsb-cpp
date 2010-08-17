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

#include "util/TaskExecutor.h"

#include "util/Task.h"
#include "util/PeriodicTask.h"
#include <boost/bind.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <iostream>

using namespace std;
using namespace rsb::util;

class PeriodicTaskTest {
public:
	PeriodicTaskTest() : i(0) {};

	boost::recursive_mutex m;
	boost::condition cond;

	void execute(Task<void> *t) {
		boost::recursive_mutex::scoped_lock lock(m);
		i++;
		cout << i << " " << flush;
		if (i==10) {
			cout << endl;
			t->cancel();
		}
	}

private:
	int i;

};

int main(void) {
	TaskExecutorVoidPtr exec(new TaskExecutor<void>());
	TaskPtr task = exec->schedule<PeriodicTaskTest>(new PeriodicTaskTest(),0);
	exec->join(task);
}
