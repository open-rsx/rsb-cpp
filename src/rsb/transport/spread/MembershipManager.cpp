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
using namespace rsc::logging;

namespace rsb {

namespace spread {

MembershipManager::MembershipManager() : logger(Logger::getLogger("rsb.spread.MembershipManager")) {
	RSBTRACE(logger, "MembershipManager() entered");
	groups = boost::shared_ptr< GroupMap >(new GroupMap());
}

MembershipManager::~MembershipManager() {
	// TODO Auto-generated destructor stub
}

void MembershipManager::join(string group, SpreadConnectionPtr c) {
	RSBDEBUG(logger, "Trying to join group with id " << group << " on SpreadConnection " << c);
	GroupMap::iterator i = groups->find(group);
	if (groups->end() != i) {
		int refs = i->second.second;
		RSBDEBUG(logger, "Group object already existing, ref count: " << refs);
		if (0 == refs) {
			// if count == 0 we re-join the group
			i->second.first->join(c);
			RSBINFO(logger, "Re-joined SpreadGroup with id " << group);
		}
		i->second.second = ++refs;
		RSBTRACE(logger, "New group ref count: " << i->second.second);
	} else {
		RSBDEBUG(logger, "Group object not found, creating new one");
		SpreadGroupPtr sg(new SpreadGroup(group));
		sg->join(c);
		RSBINFO(logger, "Joined SpreadGroup with id " << group);
		(*groups)[group] = make_pair(sg,1);
	}
}

void MembershipManager::leave(string group, SpreadConnectionPtr c) {
	RSBDEBUG(logger, "Checking if we want to leave group with id " << group << " on SpreadConnection " << c);
	GroupMap::iterator i = groups->find(group);
	if (groups->end() != i) {
		int refs = i->second.second;
		RSBDEBUG(logger, "Group object found, ref count: " << refs);
		// if count-1 == 0 sg->leave
		i->second.second = --refs;
		RSBTRACE(logger, "New group ref count: " << i->second.second);
		if (0 == refs) {
			cout << "Count is 0, leaving group" << endl;
			i->second.first->leave(c);
			RSBINFO(logger, "Left SpreadGroup with id " << group);
		}
	} else {
		RSBWARN(logger, "SpreadGroup with id " << group << " not found in GroupMap");
	}
}

}

}
