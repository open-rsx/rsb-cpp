/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#pragma once

#include <iostream>
#include <ostream>
#include <stdexcept>

#include <boost/thread.hpp>

#include <rsc/logging/LoggerFactory.h>
#include <rsc/subprocess/Subprocess.h>

#include "testconfig.h"

inline void setupLogging() {

	rsc::logging::LoggerFactory::getInstance()->reconfigure(
			rsc::logging::Logger::TRACE);

}

inline rsc::subprocess::SubprocessPtr startSpread() {
	std::vector<std::string> spreadArgs;
	spreadArgs.push_back("-n");
	spreadArgs.push_back("localhost");
	spreadArgs.push_back("-c");
	spreadArgs.push_back(TEST_ROOT + "/spread.conf");
	std::cout << "Calling " << SPREAD_EXECUTABLE << " with args:";
	for (std::vector<std::string>::iterator it = spreadArgs.begin(); it
			!= spreadArgs.end(); ++it) {
		std::cout << *it << ", ";
	}
	std::cout << std::endl;
	rsc::subprocess::SubprocessPtr proc =
			rsc::subprocess::Subprocess::newInstance(SPREAD_EXECUTABLE,
					spreadArgs);
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	return proc;
}

