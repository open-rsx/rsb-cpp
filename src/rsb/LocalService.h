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

#include <boost/thread/recursive_mutex.hpp>

#include "Service.h"
#include "rsb/rsbexports.h"

namespace rsb {

/**
 * A Service implementation that structures services locally.
 *
 * @author jwienke
 */
class RSB_EXPORT LocalService: public Service {
public:

    LocalService(const Scope &scope);
    virtual ~LocalService();

    std::set<ParticipantPtr> getParticipants() const;
    void addParticipant(ParticipantPtr participant);
    void removeParticipant(ParticipantPtr participant);
    std::set<ServicePtr> getSubServices() const;
    void addSubService(ServicePtr subService);
    void removeSubService(ServicePtr subService);

private:

    mutable boost::recursive_mutex mutex;
    std::set<ParticipantPtr> participants;
    std::set<ServicePtr> subServices;

};

}

