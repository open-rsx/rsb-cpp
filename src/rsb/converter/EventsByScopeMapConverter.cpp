/* ============================================================
 *
 * This file is a part of the RSB TimeSync project.
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

#include "EventsByScopeMapConverter.h"

#include <map>
#include <vector>

#include "../Event.h"
#include "../EventCollections.h"
#include "../Scope.h"
#include "../protocol/Notification.h"
#include "../protocol/collections/EventsByScopeMap.pb.h"
#include "Converter.h"
#include "ProtocolBufferConverter.h"
#include "SerializationException.h"

using namespace std;

namespace rsb {
namespace converter {

EventsByScopeMapConverter::EventsByScopeMapConverter(
        ConverterSelectionStrategy<std::string>::Ptr serializationConverters,
        ConverterSelectionStrategy<std::string>::Ptr deserializationConverters) :
        Converter<string>("dummy", RSB_TYPE_TAG(EventsByScopeMap)), serializationConverters(
                serializationConverters), deserializationConverters(
                deserializationConverters), converter(
                new ProtocolBufferConverter<
                        protocol::collections::EventsByScopeMap>) {

}

EventsByScopeMapConverter::~EventsByScopeMapConverter() {
}

string EventsByScopeMapConverter::getWireSchema() const {
    return converter->getWireSchema();
}

string EventsByScopeMapConverter::getClassName() const {
    return "EventsByScopeMapConverter";
}

string EventsByScopeMapConverter::serialize(const AnnotatedData& data,
        string& wire) {

    if (data.first != getDataType()) {
        throw SerializationException(
                "Called with unsupported data type " + data.first);
    }

    boost::shared_ptr<EventsByScopeMap> dataMap = boost::static_pointer_cast<
            EventsByScopeMap>(data.second);

    boost::shared_ptr<protocol::collections::EventsByScopeMap> syncMap(
            new protocol::collections::EventsByScopeMap);

    // iterate over all scopes
    for (EventsByScopeMap::const_iterator mapIt = dataMap->begin();
            mapIt != dataMap->end(); ++mapIt) {

        protocol::collections::EventsByScopeMap::ScopeSet* scopeSet =
                syncMap->add_sets();
        scopeSet->set_scope(mapIt->first.toString());

        // iterate over all events in one scope
        for (vector<EventPtr>::const_iterator eventIt = mapIt->second.begin();
                eventIt != mapIt->second.end(); ++eventIt) {

            // convert event to notification
            EventPtr event = *eventIt;

            Converter<string>::Ptr c = serializationConverters->getConverter(
                    event->getType());
            string wire;
            string wireSchema = c->serialize(
                    make_pair(event->getType(), event->getData()), wire);

            protocol::Notification* notification =
                    scopeSet->add_notifications();
            protocol::fillNotificationId(*notification, event);
            protocol::fillNotificationHeader(*notification, event, wireSchema);
            notification->set_data(wire);

        }

    }

    return converter->serialize(
            make_pair(
                    rsc::runtime::typeName<
                            protocol::collections::EventsByScopeMap>(),
                    syncMap), wire);

}

AnnotatedData EventsByScopeMapConverter::deserialize(const string& wireSchema,
        const string& wire) {

    if (wireSchema != getWireSchema()) {
        throw SerializationException("Unexpected wire schema " + wireSchema);
    }

    protocol::collections::EventsByScopeMap syncMap;
    syncMap.ParseFromString(wire);

    boost::shared_ptr<EventsByScopeMap> dataMap(new EventsByScopeMap);

    // iterate over all scope sets
    for (int setCount = 0; setCount < syncMap.sets_size(); ++setCount) {

        const protocol::collections::EventsByScopeMap::ScopeSet& scopeSet =
                syncMap.sets(setCount);
        ScopePtr scope(new Scope(scopeSet.scope()));

        // iterate over all notifications in each scope set
        for (int notificationIndex = 0;
                notificationIndex < scopeSet.notifications_size();
                ++notificationIndex) {

            // convert the notification back to an event
            const protocol::Notification& notification = scopeSet.notifications(
                    notificationIndex);

            EventPtr event(new Event);
            event->setScopePtr(scope);
            AnnotatedData annotatedData =
                    deserializationConverters->getConverter(
                            notification.wire_schema())->deserialize(
                            notification.wire_schema(), notification.data());
            event->setType(annotatedData.first);
            event->setData(annotatedData.second);

            protocol::fillEvent(event, notification, annotatedData.second,
                    annotatedData.first);

            (*dataMap)[*scope].push_back(event);

        }

    }

    return make_pair(getDataType(), dataMap);

}

}
}
