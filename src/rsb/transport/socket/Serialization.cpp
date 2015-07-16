/* ============================================================
 *
 * This file is part of the RSB project.
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "Serialization.h"

#include "../../MetaData.h"
#include "../../EventId.h"
#include "../../Scope.h"

using namespace std;

namespace rsb {
namespace transport {
namespace socket {

EventPtr notificationToEvent(protocol::Notification& notification,
                             bool                    exposeWireSchema) {
    /** TODO(jmoringe): it may be possible to keep a single event
     * instance here since connectors probably have to copy events  */
    EventPtr event(new Event());

    MetaData& metaData = event->mutableMetaData();
    metaData.setCreateTime((boost::uint64_t) notification.meta_data().create_time());
    metaData.setSendTime((boost::uint64_t) notification.meta_data().send_time());
    metaData.setReceiveTime();

    event->setId(
        rsc::misc::UUID(
            (boost::uint8_t*) notification.event_id().sender_id().c_str()),
        notification.event_id().sequence_number());
    event->setScopePtr(ScopePtr(new Scope(notification.scope())));
    if (notification.has_method()) {
        event->setMethod(notification.method());
    }

    for (int i = 0; i < notification.meta_data().user_infos_size(); ++i) {
        metaData.setUserInfo(notification.meta_data().user_infos(i).key(),
                             notification.meta_data().user_infos(i).value());
    }
    for (int i = 0; i < notification.meta_data().user_times_size(); ++i) {
        metaData.setUserTime(notification.meta_data().user_times(i).key(),
                             notification.meta_data().user_times(i).timestamp());
    }
    for (int i = 0; i < notification.causes_size(); ++i) {
        event->addCause(EventId(rsc::misc::UUID((boost::uint8_t*) notification.causes(i).sender_id().c_str()),
                                notification.causes(i).sequence_number()));
    }

    event->setData(boost::shared_ptr<string>(new string(notification.data())));

    if (exposeWireSchema) {
        metaData.setUserInfo("rsb.wire-schema", notification.wire_schema());
    }

    return event;
}

void eventToNotification(protocol::Notification& notification,
                         const EventPtr&         event,
                         const string&           wireSchema,
                         const string&           data) {
    notification.mutable_event_id()->set_sender_id(
        event->getId().getParticipantId().getId().data,
        event->getId().getParticipantId().getId().size());
    notification.mutable_event_id()->set_sequence_number(
        event->getId().getSequenceNumber());
    notification.set_scope(event->getScopePtr()->toString());
    if (!event->getMethod().empty()) {
        notification.set_method(event->getMethod());
    }
    notification.set_wire_schema(wireSchema);
    notification.mutable_meta_data()->set_create_time(
        event->getMetaData().getCreateTime());
    notification.mutable_meta_data()->set_send_time(
        event->getMetaData().getSendTime());
    for (map<string, string>::const_iterator it =
             event->mutableMetaData().userInfosBegin(); it
             != event->mutableMetaData().userInfosEnd(); ++it) {
        protocol::UserInfo* info =
            notification.mutable_meta_data()->mutable_user_infos()->Add();
        info->set_key(it->first);
        info->set_value(it->second);
    }
    for (map<string, boost::uint64_t>::const_iterator it =
             event->mutableMetaData().userTimesBegin(); it
             != event->mutableMetaData().userTimesEnd(); ++it) {
        protocol::UserTime* info =
            notification.mutable_meta_data()->mutable_user_times()->Add();
        info->set_key(it->first);
        info->set_timestamp(it->second);
    }

    set<EventId> causes = event->getCauses();
    for (set<EventId>::const_iterator it = causes.begin();
         it != causes.end(); ++it) {
        protocol::EventId* cause = notification.mutable_causes()->Add();
        cause->set_sender_id(it->getParticipantId().getId().data,
                             it->getParticipantId().getId().size());
        cause->set_sequence_number(it->getSequenceNumber());
    }

    notification.set_data(data);
}

}
}
}
