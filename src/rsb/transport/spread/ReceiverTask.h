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

#pragma once

#include <map>

#include <boost/thread.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/threading/RepetitiveTask.h>
#include <rsc/misc/UUID.h>

#include "../../protocol/Notification.h"
#include "../../protocol/FragmentedNotification.h"
#include "../../converter/Repository.h"
#include "../../Event.h"
#include "../../eventprocessing/Handler.h"
#include "../Connector.h"
#include "../../converter/Converter.h"
#include "SpreadConnection.h"
#include "Assembly.h"

namespace rsb {
namespace spread {

class InConnector;

/**
 * A task that receives FragmentedNotifications from a SpreadConnection, deserializes them
 * to events and notifies a Handler with deserialized Events. Messages may be
 * split into multiple FragmentedNotifications to respect the spread limitations. Hence,
 * there is an assembly strategy for multiple notifications forming one Event.
 * An optional pruning for Event fragments may be enable to avoid a growing pool
 * if FragmentedNotifications are lost. As a default this pruning is disabled.
 *
 * @author swrede
 * @author jwienke
 */
class ReceiverTask: public rsc::threading::RepetitiveTask {
public:

    ReceiverTask(SpreadConnectionPtr s, eventprocessing::HandlerPtr handler,
            InConnector* connector);
    virtual ~ReceiverTask();

    void execute();
    void setHandler(eventprocessing::HandlerPtr handler);

    /**
     * Enables or disables pruning of messages and waits until the changes are
     * performed. Thread-safe method.
     *
     * @param pruning if @c true and not pruning, start pruning, else if @c false
     *        and pruning, stop pruning
     */
    void setPruning(const bool& pruning);

private:

    /**
     * Notifies the handler of this task about a received event which is
     * generated from an internal notification and the joined data that may
     * originate from several fragments of the notification.
     *
     * @param notification notification with full data to notify about
     */
    void notifyHandler(protocol::NotificationPtr notification);

    /**
     * Handles newly received notifications by extracting their data, joining
     * the data if it is part of a multi-part message and returning the
     * joined notification.
     *
     * @param notification notification to handler
     * @return pointer to the joined notification
     */
    rsb::protocol::NotificationPtr handleAndJoinFragmentedNotification(
            protocol::FragmentedNotificationPtr notification);

    rsc::logging::LoggerPtr logger;
    SpreadConnectionPtr con;
    InConnector* connector;
    AssemblyPoolPtr assemblyPool;
    boost::recursive_mutex handlerMutex;
    eventprocessing::HandlerPtr handler;

};

}
}
