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

#include "eventprocessing/InRouteConfigurator.h"
#include "eventprocessing/PushInRouteConfigurator.h"

using namespace std;

using namespace rsb::transport;

namespace rsb {

class Listener::Impl {
public:
    rsc::logging::LoggerPtr logger;
    std::set<HandlerPtr> handlers;
    eventprocessing::PushInRouteConfiguratorPtr configurator;
};

Listener::Listener(const vector<InPushConnectorPtr> &connectors,
        const Scope &scope, const ParticipantConfig &config) :
    Participant(scope, config), d(new Impl) {

    d->logger = rsc::logging::Logger::getLogger("rsb.Listener");
    this->initialize(connectors, scope, config);
}

Listener::~Listener() {
}

string Listener::getClassName() const {
    return "Listener";
}

void Listener::initialize(const vector<InPushConnectorPtr> &connectors,
        const Scope &scope, const ParticipantConfig &config) {
    d->configurator.reset(
            new eventprocessing::PushInRouteConfigurator(scope, config));
    d->configurator->setErrorStrategy(getConfig().getErrorStrategy());
    for (vector<InPushConnectorPtr>::const_iterator it = connectors.begin(); it
            != connectors.end(); ++it) {
        d->configurator->addConnector(*it);
    }

    d->configurator->activate();
}

void Listener::addHandler(HandlerPtr h, bool wait) {
    d->configurator->handlerAdded(h, wait);
}

void Listener::removeHandler(HandlerPtr h, bool wait) {
    d->configurator->handlerRemoved(h, wait);
}

void Listener::addFilter(filter::FilterPtr filter) {
    d->configurator->filterAdded(filter);
}

void Listener::removeFilter(filter::FilterPtr filter) {
    d->configurator->filterRemoved(filter);
}

}
