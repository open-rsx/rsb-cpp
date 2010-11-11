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

#ifndef TESTHELPERS_H_
#define TESTHELPERS_H_

#include <iostream>
#include <ostream>
#include <stdexcept>

#include <rsc/logging/LoggerFactory.h>

#include "testconfig.h"

class SpreadStarter {
private:
	pid_t spreadPid;

public:

	SpreadStarter() {

		pid_t pid = vfork();
		if (pid == 0) {

			// child process, start the database here
			execl(SPREAD_EXECUTABLE.c_str(), SPREAD_EXECUTABLE.c_str(), "-n",
					"localhost", "-c", (TEST_ROOT + "/spread.conf").c_str(),
					(char *) 0);

		} else if (pid < 0) {

			// failed to fork
			throw std::runtime_error("Cannot start spread server");

		} else {

			spreadPid = pid;
			// Code only executed by parent process
			sleep(2);

		}

	}

	~SpreadStarter() {

		std::cout << "Killing spread" << std::endl;

		kill(spreadPid, SIGINT);
		int status;
		pid_t terminated;
		while (!(terminated = waitpid(spreadPid, &status, WNOHANG))) {
			continue;
		}

	}

};

inline void setupLogging() {

	rsc::logging::LoggerFactory::getInstance()->reconfigure(
			rsc::logging::Logger::TRACE);

}

#endif /* TESTHELPERS_H_ */
