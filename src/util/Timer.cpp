/*
 * Timer.cpp
 *
 *  Created on: 28.07.2010
 *      Author: swrede
 */

#include "Timer.h"

#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <log4cxx/logger.h>

using namespace std;

using namespace log4cxx;

namespace {
log4cxx::LoggerPtr log(Logger::getLogger("Timer"));
}

namespace rsb {

namespace util {

Timer::Timer() :
	name(""), running(false) {
}

Timer::Timer(string id) :
	name(id), running(false) {
}

Timer::~Timer() {
	log->debug("destructor called");
}

void Timer::start() {
	gettimeofday(&tv_start, &tz);
	running = true;
}

void Timer::stop() {
	gettimeofday(&tv_end, &tz);
	running = false;
}

void Timer::reset() {
	running = false;
	tv_end = tv_start;
}

double Timer::getElapsed() {
	double ms = 0;
	// is reset, return 0
	if ((tv_end.tv_usec == tv_start.tv_usec) && (tv_end.tv_sec
			== tv_start.tv_sec))
		return ms;
	if (running) {
		struct timeval tv_interim;
		gettimeofday(&tv_interim, &tz);
		double t1, t2;
		t1 = (double) tv_start.tv_sec + (double) tv_start.tv_usec / (1000
				* 1000);
		t2 = (double) tv_interim.tv_sec + (double) tv_interim.tv_usec / (1000
				* 1000);

		ms = (t2 - t1) * 1000;
	} else {
		double t1, t2;
		t1 = (double) tv_start.tv_sec + (double) tv_start.tv_usec / (1000
				* 1000);
		t2 = (double) tv_end.tv_sec + (double) tv_end.tv_usec / (1000 * 1000);
		ms = (t2 - t1) * 1000;
	}
	return ms;
}

string Timer::getName() {
	return name;
}

}

}
