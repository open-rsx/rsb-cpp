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

#include <stdexcept>

#include <sp.h>

#include <rsc/logging/Logger.h>

#include "../../CommException.h"

using namespace std;
using namespace rsc::logging;

namespace rsb {
namespace spread {

SpreadGroup::SpreadGroup(const string& n) :
    name(n), logger(Logger::getLogger("rsb.spread.SpreadGroup")) {
    RSCDEBUG(logger, "new spread group object, group name: " << n);
}

SpreadGroup::~SpreadGroup() {
    RSCDEBUG(logger, "destructor called for group object: " << name);
}

string SpreadGroup::getName() const {
    return name;
}

void SpreadGroup::join(SpreadConnectionPtr con) {

    if (!con->isActive()) {
        throw runtime_error("Spread connection is not active");
    }

    int retCode = SP_join(*con->getMailbox(), name.c_str());
    handleRetCode(retCode, "joining");

}

void SpreadGroup::leave(SpreadConnectionPtr con) {

    if (!con->isActive()) {
        throw CommException("Connection is not active, cannot leave " + name);
    }

    int retCode = SP_leave(*con->getMailbox(), name.c_str());
    handleRetCode(retCode, "leaving");

}

void SpreadGroup::handleRetCode(const int& retCode,
        const std::string& actionName) {

    if (!retCode) {
        RSCDEBUG(logger, actionName << " spread group with name " << name << " successful");
    } else {
        stringstream msg;
        msg << "Got error while " << actionName << " spread group '" << name
                << "': ";
        switch (retCode) {
        case ILLEGAL_GROUP:
            msg << "ILLEGAL_GROUP";
            break;
        case ILLEGAL_SESSION:
            msg << "ILLEGAL_SESSION";
            break;
        case CONNECTION_CLOSED:
            msg << "CONNECTION_CLOSED";
            break;
        default:
            msg << "Unknown spread error with code " << retCode;
            break;
        }
        RSCERROR(logger, "Error " << actionName << " spread group: " << msg.str());
        throw CommException(msg.str());
    }

}

}
}
