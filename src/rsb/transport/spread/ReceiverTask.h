/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include <map>

#include <boost/thread.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/threading/RepetitiveTask.h>
#include <rsc/misc/UUID.h>

#include "../../protocol/Notification.h"
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
 * A task that receives Notifications from a SpreadConnection, deserializes them
 * to events and notifies a Handler with deserialized Events. Messages may be
 * split into multiple Notifications to respect the spread limitations. Hence,
 * there is an assembly strategy for multiple notifications forming one Event.
 * An optional pruning for Event fragments may be enable to avoid a growing pool
 * if Notifications are lost. As a default this pruning is disabled.
 *
 * @author swrede
 * @author jwienke
 */
class ReceiverTask: public rsc::threading::RepetitiveTask {
public:

    ReceiverTask(SpreadConnectionPtr s, eventprocessing::HandlerPtr handler,
            InConnector *connector);
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
    void setPruning(const bool &pruning);

private:

    /**
     * Notifies the handler of this task about a received event which is
     * generated from an internal notification and the joined data that may
     * originate from several notifications.
     *
     * @param notification notification with meta infos
     * @param data user data for the event
     */
    void notifyHandler(protocol::NotificationPtr notification,
            boost::shared_ptr<std::string> data);

    /**
     * Handles newly received notifications by extracting their data, joining
     * the data if it is part of a multi-part message and and returning the
     * joined data if a multi-part message was received completely.
     *
     * @param notification notification to handler
     * @return pointer to joined data in case of a single-part message or a
     *         completed multi-part message. If no event data is completely
     *         available, a null pointer is returned
     */
    boost::shared_ptr<std::string> handleAndJoinNotification(
            protocol::NotificationPtr notification);

    rsc::logging::LoggerPtr logger;
    volatile bool cancelRequested;
    SpreadConnectionPtr con;
    InConnector *connector;
    AssemblyPoolPtr assemblyPool;
    boost::recursive_mutex handlerMutex;
    eventprocessing::HandlerPtr handler;

};

}
}
