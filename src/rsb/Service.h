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

#include <set>

#include <boost/shared_ptr.hpp>

#include "Participant.h"
#include "Scope.h"

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
class Service {
public:

    virtual ~Service();

    /**
     * Returns the scope of this service under which all participants and
     * sub-service must exist.
     *
     * @return scope of this service
     */
    virtual Scope getScope() const;

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

    /**
     * Returns all currently registered sub-services.
     *
     * @return sub-services
     */
    virtual std::set<ServicePtr> getSubServices() const = 0;

    /**
     * Registers a sub-service under this service.
     *
     * @param subService service to register
     * @throw std::invalid_argument given sub-service is not in a sub-scope of
     *                              this service
     */
    virtual void addSubService(ServicePtr subService) = 0;

    /**
     * Removes a previously registered sub-service.
     *
     * @param subService service to remove
     */
    virtual void removeSubService(ServicePtr subService) = 0;

protected:

    /**
     * Constructs a new service with the given scope.
     *
     * @param scope scope of the service
     */
    Service(const Scope &scope);

private:

    Scope scope;

};

}

