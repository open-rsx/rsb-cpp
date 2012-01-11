/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include "ReceiverTask.h"

#include <rsc/misc/langutils.h>
#include <rsc/debug/DebugTools.h>

#include "../../converter/Converter.h"
#include "../../CommException.h"
#include "../../MetaData.h"
#include "../../EventId.h"
#include "SpreadConnection.h"
#include "InConnector.h"

using namespace std;
using namespace rsb;
using namespace rsb::protocol;
using namespace rsb::transport;
using namespace rsb::eventprocessing;
using namespace rsc::logging;

namespace rsb {
namespace spread {

ReceiverTask::ReceiverTask(SpreadConnectionPtr s, HandlerPtr handler,
        InConnector* connector) :
        logger(rsc::logging::Logger::getLogger("rsb.spread.ReceiverTask")), con(
                s), connector(connector), assemblyPool(new AssemblyPool()), handler(
                handler) {

    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    RSCTRACE(logger, "ReceiverTask::RecieverTask, SpreadConnection: " << con);

}

ReceiverTask::~ReceiverTask() {
}

void ReceiverTask::execute() {
    // TODO Do performance optimization for data joining
    try {

        SpreadMessagePtr message(new SpreadMessage(SpreadMessage::REGULAR));
        con->receive(message);
        if (!message) {
            throw CommException(
                    "Receiving a SpreadMessage returned a zero pointer, why?");
        }

        RSCDEBUG(logger,
                "ReceiverTask::execute new SpreadMessage received " << message);

        if (message->getType() != SpreadMessage::REGULAR) {
            return;
        }

        FragmentedNotificationPtr notification(new FragmentedNotification());
        if (!notification->ParseFromString(message->getDataAsString())) {
            throw CommException("Failed to parse notification in pbuf format");
        }

        RSCTRACE(
                logger,
                "Parsed event seqnum: " << notification->notification().event_id().sequence_number());
        RSCTRACE(
                logger,
                "Binary length: " << notification->notification().data().length());
        RSCTRACE(
                logger,
                "Number of split message parts: " << notification->num_data_parts());
        RSCTRACE(logger,
                "... received message part    : " << notification->data_part());

        // Build data from parts
        NotificationPtr completeNotification =
                handleAndJoinFragmentedNotification(notification);
        if (completeNotification) {
            RSCTRACE(
                    logger,
                    "ReceiverTask::execute fragmented notification joined, last message " << message);
            notifyHandler(completeNotification);
        }

    } catch (rsb::CommException& e) {
        // TODO QoS would not like swallowing the exception
        // TODO maybe at least use the ErrorHandlingStrategy here?
        rsc::debug::DebugToolsPtr tools = rsc::debug::DebugTools::newInstance();
        RSCERROR(
                logger,
                "Error receiving spread message: " << e.what() << endl << tools->exceptionInfo(e));
    } catch (boost::thread_interrupted& e) {
        return;
    }

}

NotificationPtr ReceiverTask::handleAndJoinFragmentedNotification(
        FragmentedNotificationPtr notification) {

    NotificationPtr completeNotification;

    bool multiPartNotification = notification->num_data_parts() > 1;
    if (multiPartNotification) {
        completeNotification = this->assemblyPool->add(notification);
    } else {
        completeNotification.reset(
                notification->mutable_notification(),
                rsc::misc::ParentSharedPtrDeleter
                        < rsb::protocol::FragmentedNotification
                        > (notification));
    }
    return completeNotification;

}

void ReceiverTask::notifyHandler(NotificationPtr notification) {

    EventPtr e(new Event());

    // TODO fix error handling, see #796
    try {
        InConnector::ConverterPtr c = this->connector->getConverter(
                notification->wire_schema());
        converter::AnnotatedData deserialized = c->deserialize(
                notification->wire_schema(), notification->data());

        fillEvent(e, *notification, deserialized.second, deserialized.first);

        e->mutableMetaData().setReceiveTime();

        boost::recursive_mutex::scoped_lock lock(handlerMutex);
        if (this->handler) {
            this->handler->handle(e);
        }
    } catch (const std::exception& ex) {
        RSCWARN(logger, "ReceiverTask::notifyHandler catched std exception: " << ex.what() );
    } catch (...) {
        RSCWARN(logger, "ReceiverTask::notifyHandler catched unknown exception" );
    }

}

void ReceiverTask::setHandler(HandlerPtr handler) {
    boost::recursive_mutex::scoped_lock lock(handlerMutex);
    this->handler = handler;
}

void ReceiverTask::setPruning(const bool& pruning) {
    assemblyPool->setPruning(pruning);
}

}
}
