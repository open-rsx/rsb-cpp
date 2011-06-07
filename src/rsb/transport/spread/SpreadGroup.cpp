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
    if (!retCode) {
        RSCDEBUG(logger, "joined spread group with name: " << name);
    } else {
        stringstream msg;
        msg << "Got error while joining spread group '" << name << "': ";
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
        RSCERROR(logger, "Error joining spread group: " << msg.str());
        // TODO real exception needed
        throw runtime_error(msg.str());
    }

}

void SpreadGroup::leave(SpreadConnectionPtr con) {
    if (!con->isActive()) {
        throw runtime_error("Connection is not active, cannot leave " + name);
    }

    // TODO evaluate error codes and membership message
    SP_leave(*con->getMailbox(), name.c_str());
    RSCDEBUG(logger, "left spread group with name: " << name);
}

}

}
