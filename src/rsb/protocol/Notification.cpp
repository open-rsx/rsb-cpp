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

#include "Notification.h"

#include "../Event.h"
#include "../EventId.h"
#include "../MetaData.h"
#include "../Scope.h"

using namespace std;

namespace rsb {
namespace protocol {

void fillEventId(protocol::EventId& id, const rsb::EventId& realId) {
    boost::uuids::uuid uuid = realId.getParticipantId().getId();
    id.set_sender_id(uuid.data, uuid.size());
    id.set_sequence_number(realId.getSequenceNumber());
}

void fillNotificationId(protocol::Notification& notification,
        const EventPtr& event) {
    fillEventId(*(notification.mutable_event_id()), event->getId());
}

void fillNotificationHeader(protocol::Notification& notification,
        const EventPtr& event, const string& wireSchema) {

    notification.set_wire_schema(wireSchema);

    notification.set_scope(event->getScopePtr()->toString());
    if (!event->getMethod().empty()) {
        notification.set_method(event->getMethod());
    }
    notification.mutable_meta_data()->set_create_time(
            event->getMetaData().getCreateTime());
    notification.mutable_meta_data()->set_send_time(
            event->getMetaData().getSendTime());
    notification.mutable_meta_data()->set_receive_time(
            event->getMetaData().getReceiveTime());
    notification.mutable_meta_data()->set_deliver_time(
            event->getMetaData().getDeliverTime());
    for (map<string, string>::const_iterator it =
            event->mutableMetaData().userInfosBegin();
            it != event->mutableMetaData().userInfosEnd(); ++it) {
        UserInfo* info =
                notification.mutable_meta_data()->mutable_user_infos()->Add();
        info->set_key(it->first);
        info->set_value(it->second);
    }
    for (map<string, boost::uint64_t>::const_iterator it =
            event->mutableMetaData().userTimesBegin();
            it != event->mutableMetaData().userTimesEnd(); ++it) {
        UserTime* info =
                notification.mutable_meta_data()->mutable_user_times()->Add();
        info->set_key(it->first);
        info->set_timestamp(it->second);
    }
    set<rsb::EventId> causes = event->getCauses();
    for (set<rsb::EventId>::const_iterator causeIt = causes.begin();
            causeIt != causes.end(); ++causeIt) {
        fillEventId(*(notification.add_causes()), *causeIt);
    }

}

void fillEvent(EventPtr event, const protocol::Notification& notification,
        VoidPtr data, const string& dataType) {

    event->mutableMetaData().setCreateTime(
            (boost::uint64_t) notification.meta_data().create_time());
    event->mutableMetaData().setSendTime(
            (boost::uint64_t) notification.meta_data().send_time());
    event->mutableMetaData().setReceiveTime(
            (boost::uint64_t) notification.meta_data().receive_time());
    event->mutableMetaData().setDeliverTime(
            (boost::uint64_t) notification.meta_data().deliver_time());
    event->setId(
            rsc::misc::UUID(
                    (boost::uint8_t*) notification.event_id().sender_id().c_str()),
            notification.event_id().sequence_number());

    event->setScopePtr(ScopePtr(new Scope(notification.scope())));
    if (notification.has_method()) {
        event->setMethod(notification.method());
    }

    for (int i = 0; i < notification.meta_data().user_infos_size(); ++i) {
        event->mutableMetaData().setUserInfo(
                notification.meta_data().user_infos(i).key(),
                notification.meta_data().user_infos(i).value());
    }
    for (int i = 0; i < notification.meta_data().user_times_size(); ++i) {
        event->mutableMetaData().setUserTime(
                notification.meta_data().user_times(i).key(),
                notification.meta_data().user_times(i).timestamp());
    }
    for (int i = 0; i < notification.causes_size(); ++i) {
        protocol::EventId cause = notification.causes(i);
        event->addCause(
                rsb::EventId(
                        rsc::misc::UUID(
                                (boost::uint8_t*) cause.sender_id().c_str()),
                        cause.sequence_number()));
    }

    event->setType(dataType);
    event->setData(data);

}

}
}
