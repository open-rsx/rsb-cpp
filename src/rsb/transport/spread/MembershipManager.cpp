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

#include "MembershipManager.h"

#include <iostream>

using namespace std;
using namespace log4cxx;

namespace rsb {

namespace spread {

MembershipManager::MembershipManager() : logger(Logger::getLogger("rsb.spread.MembershipManager")) {
	LOG4CXX_TRACE(logger, "MembershipManager() entered");
	groups = boost::shared_ptr< GroupMap >(new GroupMap());
}

MembershipManager::~MembershipManager() {
	// TODO Auto-generated destructor stub
}

void MembershipManager::join(string group, SpreadConnectionPtr c) {
	LOG4CXX_DEBUG(logger, "Trying to join group with id " << group << " on SpreadConnection " << c);
	GroupMap::iterator i = groups->find(group);
	if (groups->end() != i) {
		int refs = i->second.second;
		LOG4CXX_DEBUG(logger, "Group object already existing, ref count: " << refs);
		if (0 == refs) {
			// if count == 0 we re-join the group
			i->second.first->join(c);
			LOG4CXX_INFO(logger, "Re-joined SpreadGroup with id " << group);
		}
		i->second.second = ++refs;
		LOG4CXX_TRACE(logger, "New group ref count: " << i->second.second);
	} else {
		LOG4CXX_DEBUG(logger, "Group object not found, creating new one");
		SpreadGroupPtr sg(new SpreadGroup(group));
		sg->join(c);
		LOG4CXX_INFO(logger, "Joined SpreadGroup with id " << group);
		(*groups)[group] = make_pair(sg,1);
	}
}

void MembershipManager::leave(string group, SpreadConnectionPtr c) {
	LOG4CXX_DEBUG(logger, "Checking if we want to leave group with id " << group << " on SpreadConnection " << c);
	GroupMap::iterator i = groups->find(group);
	if (groups->end() != i) {
		int refs = i->second.second;
		LOG4CXX_DEBUG(logger, "Group object found, ref count: " << refs);
		// if count-1 == 0 sg->leave
		i->second.second = --refs;
		LOG4CXX_TRACE(logger, "New group ref count: " << i->second.second);
		if (0 == refs) {
			cout << "Count is 0, leaving group" << endl;
			i->second.first->leave(c);
			LOG4CXX_INFO(logger, "Left SpreadGroup with id " << group);
		}
	} else {
		LOG4CXX_WARN(logger, "SpreadGroup with id " << group << " not found in GroupMap");
	}
}

}

}
