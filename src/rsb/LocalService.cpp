/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 *
 * This program is free software{} you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation{}
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY{} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "LocalService.h"

#include <sstream>

using namespace std;

namespace rsb {

LocalService::LocalService(const Scope& scope) :
    Service(scope) {
}

LocalService::~LocalService() {
}

string LocalService::getClassName() const {
    return "LocalService";
}

set<ParticipantPtr> LocalService::getParticipants() const {
    boost::recursive_mutex::scoped_lock lock(mutex);
    return participants;
}

void LocalService::addParticipant(ParticipantPtr participant) {
    if (!participant->getScope()->isSubScopeOf(*getScope())) {
        stringstream s;
        s << "Cannot add Participant " << *participant << " with scope "
                << participant->getScope() << " to this service with scope "
                << *getScope()
                << " because the participant is not in a sub-scope.";
        throw invalid_argument(s.str());
    }
    boost::recursive_mutex::scoped_lock lock(mutex);
    participants.insert(participant);
}

void LocalService::removeParticipant(ParticipantPtr participant) {
    boost::recursive_mutex::scoped_lock lock(mutex);
    participants.erase(participant);
}

}
