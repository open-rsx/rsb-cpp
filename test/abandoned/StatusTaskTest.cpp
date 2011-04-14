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
#include "rsb/transport/inprocess/InProcessConnector.h"

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>

using namespace std;
using namespace rsb;
using namespace rsb::util;
using namespace rsb::inprocess;

int main(void) {
	boost::shared_ptr<StatusTask> st = boost::shared_ptr<StatusTask>(
			new StatusTask(NULL));
	boost::shared_ptr<boost::thread> t = (TaskPtr(new Task<void> (boost::bind(
			&StatusTask::execute, st, _1))))->start();
	return EXIT_SUCCESS;
}
