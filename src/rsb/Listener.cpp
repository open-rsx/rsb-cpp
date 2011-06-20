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

#include "Listener.h"

#include <algorithm>
#include <iterator>

using namespace std;

using namespace rsb::transport;

namespace rsb {

Listener::Listener(const vector<InPushConnectorPtr> &connectors,
        const Scope &scope, const ParticipantConfig &config) :
    Participant(scope, config),
            logger(rsc::logging::Logger::getLogger("rsb.Listener")) {
    this->initialize(connectors, scope);
}

Listener::~Listener() {
}

string Listener::getClassName() const {
    return "Listener";
}

void Listener::initialize(const vector<InPushConnectorPtr> &connectors,
        const Scope &scope) {
    // TODO evaluate configuration
    this->configurator.reset(new eventprocessing::PushInRouteConfigurator(scope));
    this->configurator->setErrorStrategy(getConfig().getErrorStrategy());
    for (vector<InPushConnectorPtr>::const_iterator it = connectors.begin(); it
            != connectors.end(); ++it) {
        this->configurator->addConnector(*it);
    }

    this->configurator->activate();
}

void Listener::addHandler(HandlerPtr h, bool wait) {
    this->configurator->handlerAdded(h, wait);
}

void Listener::removeHandler(HandlerPtr h, bool wait) {
    this->configurator->handlerRemoved(h, wait);
}

void Listener::addFilter(filter::FilterPtr filter) {
    configurator->filterAdded(filter);
}

void Listener::removeFilter(filter::FilterPtr filter) {
    configurator->filterRemoved(filter);
}

}
