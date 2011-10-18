/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "PullEventReceivingStrategy.h"

#include "../filter/Filter.h"

#include "../transport/InPullConnector.h"

using namespace std;

using namespace boost;

using namespace rsb::filter;
using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

typedef std::set<transport::InPullConnectorPtr> ConnectorSet;
typedef std::set<filter::FilterPtr> FilterSet;

class PullEventReceivingStrategy::Impl {
public:
    FilterSet filters;

    ConnectorSet connectors;
    /**
     * Stores event obtained via connector callbacks.
     * */
    EventPtr currentEvent;
};

PullEventReceivingStrategy::PullEventReceivingStrategy(
        const set<InPullConnectorPtr> &connectors) : d(new Impl) {
    d->connectors = connectors;
}

PullEventReceivingStrategy::~PullEventReceivingStrategy() {
}

void PullEventReceivingStrategy::addFilter(FilterPtr filter) {
    d->filters.insert(filter);
}

void PullEventReceivingStrategy::removeFilter(FilterPtr filter) {
    d->filters.erase(filter);
}

EventPtr PullEventReceivingStrategy::raiseEvent(bool block) {
    // Go through our connectors and ask them to emit an event. If one
    // connector does emit an event, our handle method gets called and
    // may store the event if it matches our filters.
    for (ConnectorSet::iterator it = d->connectors.begin(); it
            != d->connectors.end(); ++it) {
        if ((*it)->raiseEvent(block)) {
            if (d->currentEvent) {
                EventPtr result = d->currentEvent;
                d->currentEvent.reset();
                return result;
            }
        }
    }
    return EventPtr();
}

void PullEventReceivingStrategy::handle(EventPtr event) {
    // TODO filter
    d->currentEvent = event;
}

std::string PullEventReceivingStrategy::getClassName() const {
    return "PullEventReceivingStrategy";
}

void PullEventReceivingStrategy::printContents(ostream &stream) const {
    stream << "connectors = " << d->connectors << ", filters = "
            << d->filters;
}

}
}
