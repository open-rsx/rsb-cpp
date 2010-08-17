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

#include "SpreadGroup.h"

#include <log4cxx/logger.h>

using namespace std;
using namespace log4cxx;

namespace {
LoggerPtr logger(Logger::getLogger("spread.SpreadGroup"));
}

namespace rsb {

namespace spread {

SpreadGroup::SpreadGroup(const string& n) :
	name(n) {
	logger->debug("new spread group object, group name: " + n);
}

SpreadGroup::~SpreadGroup() {
	logger->debug("destructor called");
}

string SpreadGroup::getName() const {
	return name;
}

}

}
