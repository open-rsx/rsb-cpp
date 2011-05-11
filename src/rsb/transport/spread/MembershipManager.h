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

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>

#include "SpreadConnection.h"
#include "rsb/rsbexports.h"

#pragma once

namespace rsb {
namespace spread {

typedef std::map<std::string , std::pair<SpreadGroupPtr, int> > GroupMap;

/**
 * Reference counting class for Spread group memberships.
 *
 * @author swrede
 * @todo think about adding SpreadConnectionPtr as a member, offering
 *       methods that operate on this connection only
 * @todo Threadsafety!!!
 */
class RSB_EXPORT MembershipManager {
public:
	MembershipManager();
	virtual ~MembershipManager();

	/**
	 * Joins the given Spread group if not previously done
	 * and increments reference count for this group by one.
	 *
	 * @param group
	 */
	void join(std::string group, SpreadConnectionPtr s);

	/**
	 * Decrements the reference count for the given Spread
	 * group identifier. If reference count for this identifier
	 * drops to zero, the corresponding Spread group is left.
	 *
	 * @param group
	 */
	void leave(std::string group, SpreadConnectionPtr s);

private:
	rsc::logging::LoggerPtr logger;
	boost::shared_ptr< GroupMap > groups;
};

typedef boost::shared_ptr<MembershipManager> MembershipManagerPtr;

}

}

