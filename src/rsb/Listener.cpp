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

#include "filter/ScopeFilter.h"

using namespace std;

using namespace rsb::transport;

namespace rsb {

Listener::Listener(const vector<transport::InConnectorPtr> &connectors,
        const Scope &scope, const ParticipantConfig &config) :
    Participant(scope, config), logger(rsc::logging::Logger::getLogger(
            "rsb.Listener")), passive(false) {
    this->initialize(connectors, scope);
}

Listener::~Listener() {
    if (!passive) {
        deactivate();
    }
}

string Listener::getClassName() const {
    return "Listener";
}

void Listener::initialize(const vector<InConnectorPtr> &connectors,
        const Scope &scope) {
    // TODO evaluate configuration
    this->configurator.reset(new eventprocessing::InRouteConfigurator());
    for (vector<InConnectorPtr>::const_iterator it = connectors.begin(); it
            != connectors.end(); ++it) {
        this->configurator->addConnector(*it);
    }

    this->subscription.reset(new Subscription());
    this->subscription->appendFilter(filter::FilterPtr(new filter::ScopeFilter(
            scope)));

    this->activate();
}

void Listener::activate() {
    this->configurator->activate();
    this->passive = false;
}

void Listener::deactivate() {
    if (!this->passive) {
        this->configurator->deactivate();
    }
    this->passive = true;
}

SubscriptionPtr Listener::getSubscription() {
    return this->subscription;
}

void Listener::setSubscription(SubscriptionPtr s) {
    this->configurator->unsubscribe(this->subscription);
    this->subscription = s;
    this->subscription->appendFilter(filter::FilterPtr(new filter::ScopeFilter(
            this->getScope())));
    if (!this->handlers.empty()) {
        this->configurator->subscribe(this->subscription, this->handlers);
    }
}

set<HandlerPtr> Listener::getHandlers() const {
    set<HandlerPtr> result;
    copy(this->handlers.begin(), this->handlers.end(), inserter(result,
            result.begin()));
    return result;
}

void Listener::addHandler(HandlerPtr h) {
    if (!this->handlers.empty()) {
        this->configurator->unsubscribe(this->subscription);
    }
    this->handlers.insert(h);
    this->configurator->subscribe(this->subscription, this->handlers);
}

void Listener::removeHandler(HandlerPtr h) {
    if (!this->handlers.empty()) {
        this->configurator->unsubscribe(this->subscription);
    }
    this->handlers.erase(h);
    this->configurator->subscribe(this->subscription, this->handlers);
}

}
