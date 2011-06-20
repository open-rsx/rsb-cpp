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

using namespace std;

using namespace boost;

using namespace rsb::filter;
using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

PullEventReceivingStrategy::PullEventReceivingStrategy(const set<InPullConnectorPtr> &connectors):
    connectors(connectors) {
}

void PullEventReceivingStrategy::addFilter(FilterPtr filter) {
    this->filters.insert(filter);
}

void PullEventReceivingStrategy::removeFilter(FilterPtr filter) {
    this->filters.erase(filter);
}

EventPtr PullEventReceivingStrategy::emit(bool block) {
    // Go through our connectors and ask them to emit an event. If one
    // connector does emit an event, our handle method gets called and
    // may store the event if it matches our filters.
    for (ConnectorSet::iterator it = this->connectors.begin();
         it != this->connectors.end(); ++it) {
        if ((*it)->emit(block)) {
            if (this->currentEvent) {
                EventPtr result = this->currentEvent;
                this->currentEvent.reset();
                return result;
            }
        }
    }
    return EventPtr();
}

void PullEventReceivingStrategy::handle(EventPtr event) {
    // TODO filter
    this->currentEvent = event;
}

std::string PullEventReceivingStrategy::getClassName() const {
    return "PullEventReceivingStrategy";
}

void PullEventReceivingStrategy::printContents(ostream &stream) const {
    stream << "connectors = " << this->connectors
           << ", filters = " << this->filters;
}

}
}
