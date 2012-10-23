/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2012 Jan Moringen <jmoringe@techfak.uni-bielfeld.de>
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

#include "MessageHandler.h"

#include "../../CommException.h"
#include "../../MetaData.h"
#include "../../EventId.h"

#include "../../converter/Converter.h"

using namespace std;

using namespace rsc::logging;

using namespace rsb;
using namespace rsb::transport;
using namespace rsb::protocol;

namespace rsb {
namespace transport {
namespace spread {

MessageHandler::MessageHandler(ConverterSelectionStrategyPtr converters) :
    ConverterSelectingConnector<string>(converters),
    logger(rsc::logging::Logger::getLogger("rsb.transport.spread.MessageHandler")),
    assemblyPool(new AssemblyPool()) {
}

MessageHandler::~MessageHandler() {
}

EventPtr MessageHandler::processMessage(SpreadMessagePtr message) {
    RSCDEBUG(logger, "new Spread message " << message);

    assert(message->getType() == SpreadMessage::REGULAR);

    // Deserialize notification fragment from Spread message.
    FragmentedNotificationPtr fragment(new FragmentedNotification());
    if (!fragment->ParseFromString(message->getDataAsString())) {
        throw CommException("Failed to parse notification in pbuf format");
    }

    RSCTRACE(logger,
             "Parsed event seqnum: " << fragment->notification().event_id().sequence_number());
    RSCTRACE(logger,
             "Binary length: " << fragment->notification().data().length());
    RSCTRACE(logger,
             "Number of split message parts: " << fragment->num_data_parts());
    RSCTRACE(logger,
             "... received message part    : " << fragment->data_part());

    // Build data from parts.
    NotificationPtr notification = handleAndJoinFragmentedNotification(fragment);
    if (!notification) {
        return EventPtr();
    }
    RSCTRACE(logger,
             "fragmented notification joined, last message " << message);

    // Convert notification payload.
    ConverterPtr converter = getConverter(notification->wire_schema());
    AnnotatedData deserialized = converter->deserialize(notification->wire_schema(),
                                                        notification->data());

    // Construct and return event.
    EventPtr event(new Event());
    fillEvent(event, *notification, deserialized.second, deserialized.first);
    event->mutableMetaData().setReceiveTime();

    return event;
}

NotificationPtr MessageHandler::handleAndJoinFragmentedNotification(FragmentedNotificationPtr fragment) {
    NotificationPtr notification;

    if (fragment->num_data_parts() > 1) {
        notification = this->assemblyPool->add(fragment);
    } else {
        notification.reset(fragment->mutable_notification(),
                             rsc::misc::ParentSharedPtrDeleter
                             <rsb::protocol::FragmentedNotification>(fragment));
    }
    return notification;
}

void MessageHandler::setPruning(const bool& pruning) {
    this->assemblyPool->setPruning(pruning);
}

}
}
}
