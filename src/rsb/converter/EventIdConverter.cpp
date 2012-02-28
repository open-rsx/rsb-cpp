/* ============================================================
 *
 * This file is a part of the RSB TimeSync project.
 *
 * Copyright (C) 2012 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "EventIdConverter.h"

#include <map>
#include <vector>

#include "../EventId.h"
#include "rsb/protocol/EventId.pb.h"
#include "ProtocolBufferConverter.h"
#include "SerializationException.h"

using namespace std;

namespace rsb {
namespace converter {

EventIdConverter::EventIdConverter() :
        Converter<string>("dummy", RSB_TYPE_TAG(EventId)), converter(
                new ProtocolBufferConverter<protocol::EventId>) {
}

EventIdConverter::~EventIdConverter() {
}

string EventIdConverter::getWireSchema() const {
    return converter->getWireSchema();
}

string EventIdConverter::getClassName() const {
    return "EventIdConverter";
}

string EventIdConverter::serialize(const AnnotatedData& data, string& wire) {

    if (data.first != getDataType()) {
        throw SerializationException(
                "Called with unsupported data type " + data.first);
    }

    boost::shared_ptr<EventId> eventId = boost::static_pointer_cast<EventId>(
            data.second);

    boost::shared_ptr<protocol::EventId> protocolId(new protocol::EventId);
    // TODO duplicated code with internal sending of events (Notification.cpp)
    boost::uuids::uuid uuid = eventId->getParticipantId().getId();
    protocolId->set_sender_id(uuid.data, uuid.size());
    protocolId->set_sequence_number(eventId->getSequenceNumber());

    return converter->serialize(
            make_pair(rsc::runtime::typeName<protocol::EventId>(), protocolId),
            wire);

}

AnnotatedData EventIdConverter::deserialize(const string& wireSchema,
        const string& wire) {

    boost::shared_ptr<protocol::EventId> protocolId =
            boost::static_pointer_cast<protocol::EventId>(
                    converter->deserialize(wireSchema, wire).second);

    return make_pair(
            getDataType(),
            boost::shared_ptr<EventId>(
                    new EventId(
                            rsc::misc::UUID(
                                    (boost::uint8_t*) protocolId->sender_id().c_str()),
                            protocolId->sequence_number())));

}

}
}
