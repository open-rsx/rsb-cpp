/* ============================================================
 *
 * This file is a part of the RSB project.
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

#include "Factory.h"

#include "introspection/introspection.h"
#include "converter/converters.h"
#include "transport/transports.h"
#include "LocalService.h"

using namespace std;

namespace rsb {

Factory::Factory() {
    introspection::registerIntrospectionConverters();
    converter::registerDefaultConverters();
    transport::registerDefaultTransports();

    // setup default participant config
    // TODO later this should be inprocess
    defaultConfig.addTransport(ParticipantConfig::Transport("spread"));

}

Factory::~Factory() {
}

ListenerPtr Factory::createListener(const Scope &scope,
        const ParticipantConfig &config) {
    // Create requested connectors
    std::vector<transport::InConnectorPtr> connectors;
    set<ParticipantConfig::Transport> configuredTransports =
            config.getTransports();
    for (set<ParticipantConfig::Transport>::const_iterator transportIt =
            configuredTransports.begin(); transportIt
            != configuredTransports.end(); ++transportIt) {
        connectors.push_back(
                transport::InConnectorPtr(
                        transport::InFactory::getInstance().createInst(
                                transportIt->getName(),
                                transportIt->getOptions())));
    }

    return ListenerPtr(new Listener(connectors, scope, defaultConfig));
}

patterns::ServerPtr Factory::createServer(const Scope &scope) {
    return patterns::ServerPtr(new patterns::Server(scope));
}

patterns::RemoteServerPtr Factory::createRemoteServer(const Scope &scope) {
    return patterns::RemoteServerPtr(new patterns::RemoteServer(scope));
}

ParticipantConfig Factory::getDefaultParticipantConfig() const {
    boost::recursive_mutex::scoped_lock lock(configMutex);
    return defaultConfig;
}

void Factory::setDefaultParticipantConfig(const ParticipantConfig &config) {
    boost::recursive_mutex::scoped_lock lock(configMutex);
    this->defaultConfig = config;
}

ServicePtr Factory::createService(const Scope &scope) {
    return ServicePtr(new LocalService(scope));
}

}
