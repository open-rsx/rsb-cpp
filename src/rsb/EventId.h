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

#include <boost/cstdint.hpp>
#include <boost/operators.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <rsc/misc/UUID.h>
#include <rsc/runtime/Printable.h>

#include "rsb/rsbexports.h"

namespace rsb {

/**
 * A unique ID for events in RSB. Ids are composed of the sending
 * participant's ID and a sequence number and can optionally be converted to
 * UUIDs.
 *
 * @author jwienke
 */
class RSB_EXPORT EventId: boost::totally_ordered<EventId>,
        public rsc::runtime::Printable {
public:

    EventId(const rsc::misc::UUID& participantId,
            const boost::uint32_t& sequenceNumber);
    virtual ~EventId();

    rsc::misc::UUID getParticipantId() const;
    boost::uint32_t getSequenceNumber() const;

    rsc::misc::UUID getAsUUID() const;

    bool operator==(const EventId& other) const;
    bool operator<(const EventId& other) const;

    std::string getClassName() const;
    void printContents(std::ostream& stream) const;

private:

    /**
     * The id of the sending participant.
     */
    rsc::misc::UUID participantId;
    boost::uint32_t sequenceNumber;

    /**
     * A cache for the generated uuid.
     */
    mutable rsc::misc::UUIDPtr id;

};

typedef boost::shared_ptr<EventId> EventIdPtr;

}
