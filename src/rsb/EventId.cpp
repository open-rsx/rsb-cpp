/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
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
