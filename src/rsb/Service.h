/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <ostream>
#include <set>

#include <boost/shared_ptr.hpp>

#include <rsc/runtime/Printable.h>

#include "Participant.h"
#include "Scope.h"
#include "rsb/rsbexports.h"

namespace rsb {

class Service;
typedef boost::shared_ptr<Service> ServicePtr;

/**
 * A service provides a hierarchical structure for organizing participants.
 * A service forms a parent scope for subordinated participants and
 * sub-services.
 *
 * Services need to be thread-safe.
 *
 * @author jwienke
 */
class RSB_EXPORT Service: public Participant {
public:

    virtual ~Service();

    void printContents(std::ostream& stream) const;

    /**
     * Returns all participants that reside under this service
     *
     * @return set of participants of this service
     */
    virtual std::set<ParticipantPtr> getParticipants() const = 0;

    /**
     * Registers a new participant in this service.
     *
     * @param participant participant to register
     * @throw std::invalid_argument participant is not in a sub-scope of the
     *                              service's scope
     */
    virtual void addParticipant(ParticipantPtr participant) = 0;

    /**
     * Removes a previously registered participant from this service.
     *
     * @param participant participant to remove
     */
    virtual void removeParticipant(ParticipantPtr participant) = 0;

protected:

    /**
     * Constructs a new service with the given scope.
     *
     * @param scope scope of the service
     */
    Service(const Scope& scope);

private:

    Scope scope;

};

}

