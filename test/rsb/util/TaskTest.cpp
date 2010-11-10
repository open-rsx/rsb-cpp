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
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
//#include <rsc/logging/basicconfigurator.h>

using namespace std;
using namespace rsb::util;
using namespace testing;

class PeriodicTaskTest {
public:
	PeriodicTaskTest() : i(0) {};

	boost::mutex m;
	boost::condition cond;

	void execute(Task<void>* t) {
		boost::mutex::scoped_lock lock(m);

		i++;
		// cout << i << " " << flush;
		if (i==10) {
			//cout << endl;
			// Notify main thread
			cond.notify_all();
			// TODO discuss the following observation:
			// Wait for approval by main thread, without this extra wait,
			// member i sometimes has already been incremented by one when the main thread
			// checks for i==10. Why? And why didn't it happen before switch to gtest?
			// Hypothesis: This thread is fast and the second thread is now slower. IF
			//             execute is called before the next instruction in the main
			//             it can re-acquire the lock (see above) and increment i before
			//             the comparison is done in the main thread.
			// However: Shouldn't wait re-acquire the lock atomically such that this
			//          this thread shall block in line 41?
			cond.wait(lock);
		}
		if (i>20) {
			ASSERT_TRUE(0) << "Race Condition Detected!";
		}
	}

	volatile int i;

};

TEST(TaskTest, testExecution) {
	boost::shared_ptr<PeriodicTaskTest> p = boost::shared_ptr<PeriodicTaskTest>(new PeriodicTaskTest());
	TaskPtr t(new PeriodicTask<void>(boost::bind(&PeriodicTaskTest::execute,p.get(),_1),0));

	boost::shared_ptr<boost::thread> thread = t->start();

	boost::mutex::scoped_lock lock(p->m);
	bool shutdown = false;
	while (!shutdown) {
		//cout << "Step " << p->i << endl << flush;
		while (p->i != 10) {
			p->cond.wait(lock);
		}
		//cout << "Condition satisfied in Step " << p->i << endl << flush;
		if (p->i == 10) {
			//cout << "Stopping Task!" << endl;
			t->cancel();
			lock.unlock();
			p->cond.notify_all();
			t->waitDone();
			shutdown = true;
		}
	}
	//cerr << "main finished" << endl;
}

int main(int argc, char* argv[]) {
	//log4cxx::BasicConfigurator::configure();
    InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();

}
