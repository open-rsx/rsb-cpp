/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "Participant.h"

#include "Scope.h"
#include "ParticipantConfig.h"

using namespace std;

namespace rsb {

class Participant::Impl {
public:
    rsc::misc::UUID id;
    ScopePtr scope;
    ParticipantConfig config;
};

Participant::Participant(const Scope& scope, const ParticipantConfig& config) :
    d(new Impl) {
    d->scope.reset(new Scope(scope));
    d->config = config;
}

Participant::~Participant() {
}

rsc::misc::UUID Participant::getId() const {
    return d->id;
}

ScopePtr Participant::getScope() const {
    return d->scope;
}

ParticipantConfig Participant::getConfig() const {
    return d->config;
}

void Participant::printContents(ostream& stream) const {
    stream << "id = " << d->id << ", scope = " << *d->scope;
}

}
