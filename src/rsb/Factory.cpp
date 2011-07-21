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

using namespace rsc::logging;
using namespace rsc::runtime;

using namespace rsb::converter;

namespace rsb {

Factory::Factory() :
    logger(Logger::getLogger("rsb.Factory")) {
    introspection::registerIntrospectionConverters();
    converter::registerDefaultConverters();
    transport::registerDefaultTransports();

    // setup default participant config
    this->defaultConfig = ParticipantConfig::fromConfiguration();
    if (this->defaultConfig.getTransports().empty()) {
        // TODO later this should be inprocess
        this->defaultConfig.addTransport(ParticipantConfig::Transport("spread"));
    }
    RSCDEBUG(logger, "Default config " << defaultConfig);
}

Factory::~Factory() {
}

transport::OutFactory &Factory::getOutFactoryInstance() {
    return transport::OutFactory::getInstance();
}

ListenerPtr Factory::createListener(const Scope &scope,
        const ParticipantConfig &config) {
    // Create requested connectors
    vector<transport::InPushConnectorPtr> connectors
      = createConnectors<transport::InPushFactory>(config);
    return ListenerPtr(new Listener(connectors, scope, config));
}

ReaderPtr Factory::createReader(const Scope &scope,
				const ParticipantConfig &config) {
    // Create requested connectors
    vector<transport::InPullConnectorPtr> connectors
	= createConnectors<transport::InPullFactory>(config);
    return ReaderPtr(new Reader(connectors, scope, config));
}

patterns::ServerPtr Factory::createServer(const Scope &scope) {
    return patterns::ServerPtr(new patterns::Server(scope));
}

patterns::RemoteServerPtr Factory::createRemoteServer(const Scope &scope,
                                                      unsigned int maxReplyWaitTime) {
    return patterns::RemoteServerPtr(new patterns::RemoteServer(scope, maxReplyWaitTime));
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
