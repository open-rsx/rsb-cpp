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

#include "EventQueuePushHandler.h"

using namespace std;

namespace rsb {

EventQueuePushHandler::EventQueuePushHandler(
        boost::shared_ptr<rsc::threading::SynchronizedQueue<EventPtr> > queue,
        const string& method) :
    Handler(method), queue(queue) {
}

string EventQueuePushHandler::getClassName() const {
    return "EventQueuePushHandler";
}

void EventQueuePushHandler::printContents(ostream& stream) const {
    stream << "queue = " << queue;
}

void EventQueuePushHandler::handle(EventPtr event) {
    queue->push(event);
}

}
