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

#include "EventId.h"

#include <boost/format.hpp>

using namespace std;

namespace rsb {

EventId::EventId(const rsc::misc::UUID& participantId,
        const boost::uint32_t& sequenceNumber) :
        participantId(participantId), sequenceNumber(sequenceNumber) {
}

EventId::~EventId() {
}

rsc::misc::UUID EventId::getParticipantId() const {
    return participantId;
}

boost::uint32_t EventId::getSequenceNumber() const {
    return sequenceNumber;
}

rsc::misc::UUID EventId::getAsUUID() const {
    // this does not need locking as in the worst case it may only be set twice
    // and reading a shared pointer is not affected by this because it is
    // thread-safe.
    if (!id) {
        id.reset(
                new rsc::misc::UUID(
                        participantId,
                        boost::str(
                                boost::format("%1$08x")
                                        % this->sequenceNumber)));
    }
    return *id;
}

bool EventId::operator==(const EventId& other) const {
    // first compare the sequence number which is most likely already different
    return (sequenceNumber == other.sequenceNumber)
            && (participantId == other.participantId);
}

bool EventId::operator<(const EventId& other) const {
    if (sequenceNumber < other.sequenceNumber) {
        return true;
    } else if (sequenceNumber > other.sequenceNumber) {
        return false;
    } else {
        return participantId < other.participantId;
    }
}

string EventId::getClassName() const {
    return "EventId";
}

void EventId::printContents(ostream& stream) const {
    stream << "participantId = " << participantId << ", sequenceNumber = "
            << sequenceNumber;
}

}
