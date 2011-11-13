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

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>

#include "SpreadConnection.h"

namespace rsb {
namespace spread {

/**
 * @author swrede
 */
class SpreadGroup {
public:
    SpreadGroup(const std::string& n);
    virtual ~SpreadGroup();

    std::string getName() const;
    // MembershipList getMembers();

    /**
     * Joins it's group on the given connection.
     *
     * @param con the connection to join on
     * @throw CommException spread error joining
     */
    virtual void join(SpreadConnectionPtr con);
    /**
     * Leaves it's group on the given connection.
     *
     * @param con the connection to leave on
     * @throw CommException spread error leaving
     */
    virtual void leave(SpreadConnectionPtr con);

private:

    void handleRetCode(const int& code, const std::string& actionName);

    std::string name;
    rsc::logging::LoggerPtr logger;
    SpreadConnectionPtr con;
};

typedef boost::shared_ptr<SpreadGroup> SpreadGroupPtr;

}
}

