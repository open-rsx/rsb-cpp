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

#include "Reader.h"

#include "eventprocessing/PullEventReceivingStrategy.h"
#include "eventprocessing/PullInRouteConfigurator.h"

using namespace std;

using namespace boost;

using namespace rsb::eventprocessing;
using namespace rsb::transport;

namespace rsb {

Reader::Reader(const vector<InPullConnectorPtr> &connectors,
               const Scope                  &scope,
               const ParticipantConfig      &config) :
    Participant(scope, config) {
    this->configurator.reset(new PullInRouteConfigurator(scope, config));
    for (vector<InPullConnectorPtr>::const_iterator it = connectors.begin(); it
             != connectors.end(); ++it) {
        this->configurator->addConnector(*it);
    }
    this->configurator->activate();
}

EventPtr Reader::read(bool block) {
    PullEventReceivingStrategyPtr strategy
        = dynamic_pointer_cast<PullEventReceivingStrategy>(this->configurator->getEventReceivingStrategy());
    assert(strategy);
    return strategy->emit(block);
}

string Reader::getClassName() const {
    return "Reader";
}

}
