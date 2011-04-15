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

#include <iostream>

#include <stdlib.h>
#include <math.h>

#include <boost/timer.hpp>

#include <rsc/logging/Logger.h>

#include <rsb/Informer.h>
#include <rsb/Factory.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;

int main(void) {

	Factory &factory = Factory::getInstance();

	LoggerPtr l = Logger::getLogger("informer");

	Informer<string>::Ptr informer = factory.createInformer<string> (
			"rsb://example/informer");
	Informer<string>::DataPtr s(new string("blub"));

	boost::timer t;

	for (int j = 0; j < 1200; j++) {
		informer->publish(s);
	}

	cout << "Elapsed time for " << 1200 << " messages sent: " << t.elapsed()
			<< " s" << endl;

	return EXIT_SUCCESS;
}
