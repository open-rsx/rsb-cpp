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

#ifndef SPREADGROUP_H_
#define SPREADGROUP_H_

#include "SpreadConnection.h"

#include <string>
#include <boost/shared_ptr.hpp>
#include <log4cxx/logger.h>

namespace rsb {

namespace spread {

class SpreadGroup {
public:
	SpreadGroup(const std::string& n);
	virtual ~SpreadGroup();

	std::string getName() const;
	// MembershipList getMembers();

	virtual void join(SpreadConnectionPtr con);
	virtual void leave(SpreadConnectionPtr con);

private:
	std::string name;
	log4cxx::LoggerPtr logger;
	SpreadConnectionPtr con;
};

typedef boost::shared_ptr<SpreadGroup> SpreadGroupPtr;

}

}

#endif /* SPREADGROUP_H_ */
