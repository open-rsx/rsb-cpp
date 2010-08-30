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

namespace rsb {

namespace spread {

SpreadGroup::SpreadGroup(const string& n) :
	name(n), logger(Logger::getLogger("rsb.spread.SpreadGroup")) {
	LOG4CXX_DEBUG(logger, "new spread group object, group name: " << n);
}

SpreadGroup::~SpreadGroup() {
	LOG4CXX_DEBUG(logger, "destructor called for group object: " << name);
}

string SpreadGroup::getName() const {
	return name;
}

void SpreadGroup::join(SpreadConnectionPtr con) {
	if (!con->isActive())
		throw string("not active");

	// TODO evaluate error codes
	SP_join(*con->getMailbox(), name.c_str());
	LOG4CXX_DEBUG(logger, "joined spread group with name: " << name);
}

void SpreadGroup::leave(SpreadConnectionPtr con) {
	if (!con->isActive())
		throw string("not active");

	// TODO evaluate error codes and membership message
	SP_leave(*con->getMailbox(), name.c_str());
	LOG4CXX_DEBUG(logger, "left spread group with name: " << name);
}

}

}
