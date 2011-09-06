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

#include "PullInRouteConfigurator.h"

#include <set>

#include "../transport/InPullConnector.h"
#include "PullEventReceivingStrategy.h"

using namespace std;

using namespace boost;

using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

PullInRouteConfigurator::PullInRouteConfigurator(const Scope             &scope,
                                                 const ParticipantConfig &config) :
    InRouteConfigurator(scope, config) {
}

EventReceivingStrategyPtr PullInRouteConfigurator::createEventReceivingStrategy() {
    // Pass all configured connectors a PullEventReceivingStrategy
    // object, ensuring that they actually are InPullConnectors.
    set<InConnectorPtr> connectors_ = getConnectors();
    set<InPullConnectorPtr> connectors;
    for (set<InConnectorPtr>::const_iterator it = connectors_.begin();
         it != connectors_.end(); ++it) {
        InPullConnectorPtr connector = dynamic_pointer_cast<InPullConnector>(*it);
        assert(connector);
        connectors.insert(connector);
    }
    return EventReceivingStrategyPtr(new PullEventReceivingStrategy(connectors));
}

string PullInRouteConfigurator::getClassName() const {
    return "PullInRouteConfigurator";
}

}
}
