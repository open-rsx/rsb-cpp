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

#include "InformerTask.h"

#include <rsc/misc/langutils.h>
#include <rsc/runtime/TypeStringTools.h>

using namespace std;
using namespace rsb;
using namespace rsb::transport;
using namespace rsc::threading;

namespace rsb {

namespace test {

InformerTask::InformerTask(OutConnectorPtr p, const Scope &scope,
        const unsigned int &numEvents, const unsigned int &dataSizeInBytes) :
    scope(scope), numEvents(numEvents), dataSizeInBytes(dataSizeInBytes),
            sentEvents(0), port(p) {
}

InformerTask::~InformerTask() {
}

void InformerTask::execute() {

    ++sentEvents;
    cout << "sending event: " << sentEvents << endl;
    boost::shared_ptr<string> data(
            new string(rsc::misc::randAlnumStr(dataSizeInBytes)));
    Scope thisScope = scope;
    if (sentEvents % 2 == 0) {
        // should be filtered by Port already
        thisScope = scope.concat(Scope("/foo"));
    }
    EventPtr p(new Event(thisScope, data, rsc::runtime::typeName<string>()));
    port->push(p);
    if (sentEvents % 2 == 1) {
        events.push_back(p);
    }
    if (sentEvents == 2 * numEvents) {
        cout << endl;
        cancel();
    }

}

std::vector<EventPtr> InformerTask::getEvents() {
    return events;
}

// ------

WaitingObserver::WaitingObserver(const unsigned int &desiredEvents) :
    desiredEvents(desiredEvents), receivedEvents(0) {

}

void WaitingObserver::handler(EventPtr e) {
    boost::recursive_mutex::scoped_lock lock(m);
    ++receivedEvents;
    events.push_back(e);
    cout << "Event #" << receivedEvents << "/" << desiredEvents
            << " received. Metadata: " << *e << endl;
    if (receivedEvents == desiredEvents) {
        condition.notify_all();
    }
}

vector<EventPtr> WaitingObserver::getEvents() {
    boost::recursive_mutex::scoped_lock lock(m);
    return events;
}

void WaitingObserver::waitReceived() {
    boost::recursive_mutex::scoped_lock lock(m);
    while (receivedEvents < desiredEvents) {
        condition.wait(lock);
    }
}

}
}
