/* ============================================================
 *
 * This file is a part of RSB project
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

#pragma once

#include <boost/shared_ptr.hpp>

#include <rsc/threading/SynchronizedQueue.h>

#include "Handler.h"
#include "rsb/rsbexports.h"

namespace rsb {

/**
 * A @ref rsb::Handler for @ref rsb::Listener s that pushes all
 * received events on a rsc::SynchronizedQueue.
 *
 * @author jwienke
 */
class RSB_EXPORT EventQueuePushHandler: public Handler {
public:

    /**
     * Constructs a new instance.
     *
     * @param queue the queue to push received data on
     * @param method method of this handler to react on, empty means all events
     */
    EventQueuePushHandler(
            boost::shared_ptr<rsc::threading::SynchronizedQueue<EventPtr> > queue,
            const std::string &method = "");

    std::string getClassName() const;

    void printContents(std::ostream &stream) const;

    void handle(EventPtr event);

private:
    boost::shared_ptr<rsc::threading::SynchronizedQueue<EventPtr> > queue;

};

}
