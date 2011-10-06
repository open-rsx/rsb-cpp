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

#include "eventprocessing/strategies.h"

#include "introspection/introspection.h"

#include "converter/converters.h"
#include "converter/Repository.h"
#include "converter/UnambiguousConverterMap.h"

#include "transport/transports.h"

#include "LocalService.h"

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;

using namespace rsb::converter;
using namespace rsb::transport;

namespace {

template<unsigned int which, typename C>
std::map<typename C::value_type::first_type,
         typename C::value_type::second_type> pairsToMap(const C &container) {
    typedef typename C::value_type::first_type first_type;
    typedef typename C::value_type::second_type second_type;

    typedef typename C::const_iterator const_iterator;

    std::map<first_type, second_type> result;
    for (const_iterator it = container.begin(); it != container.end(); ++it) {
        if (which == 1)
            result[it->first] = it->second;
        else
            result[it->second] = it->first;
        }
    return result;
}

}

namespace rsb {

Factory::Factory() :
    logger(Logger::getLogger("rsb.Factory")) {
    eventprocessing::registerDefaultEventProcessingStrategies();
    introspection::registerIntrospectionConverters();
    converter::registerDefaultConverters();
    transport::registerDefaultTransports();

    // Setup default participant config
    //
    // Collect all available connector implementations from the
    // connector factories:
    // + In-push
    // + In-pull
    // + Out
    // Disable discovered connectors with the exception of the
    // inprocess transport.
    set<string> availableTransports;
    {
        set<InPullFactory::ConnectorInfo> infos = InPullFactory::getInstance().getConnectorInfos();
        for (set<InPullFactory::ConnectorInfo>::const_iterator it
                 = infos.begin(); it != infos.end(); ++it) {
            availableTransports.insert(it->getName());
        }
    }{
        set<InPushFactory::ConnectorInfo> infos = InPushFactory::getInstance().getConnectorInfos();
        for (set<InPushFactory::ConnectorInfo>::const_iterator it
                 = infos.begin(); it != infos.end(); ++it) {
            availableTransports.insert(it->getName());
        }
    }{
        set<OutFactory::ConnectorInfo> infos = OutFactory::getInstance().getConnectorInfos();
        for (set<OutFactory::ConnectorInfo>::const_iterator it
                 = infos.begin(); it != infos.end(); ++it) {
            availableTransports.insert(it->getName());
        }
    }

    this->defaultConfig = ParticipantConfig();
    for (set<string>::const_iterator it = availableTransports.begin();
         it != availableTransports.end(); ++it) {
        this->defaultConfig.addTransport(ParticipantConfig::Transport(*it, *it == "inprocess"));
    }

    // Merge with user configuration (configuration files, environment
    // variables)
    this->defaultConfig
        = ParticipantConfig::fromConfiguration(this->defaultConfig);

    // Issue a warning if the combination of available transport
    // implementations and user configuration leads to no enabled
    // transports.
    if (this->defaultConfig.getTransports().empty()) {
        RSCWARN(logger, "No transports are enabled. This is probably a configuration error or an internal RSB error.");
    }

    RSCDEBUG(logger, "Default config " << defaultConfig);
}

Factory::~Factory() {
}

transport::OutFactory &Factory::getOutFactoryInstance() {
    return transport::OutFactory::getInstance();
}

InformerBasePtr Factory::createInformerBase(const Scope             &scope,
                                            const string            &dataType,
                                            const ParticipantConfig &config) {
    return InformerBasePtr(new InformerBase(createOutConnectors(config), scope, config, dataType));
}


ListenerPtr Factory::createListener(const Scope &scope,
        const ParticipantConfig &config) {
    return ListenerPtr(new Listener(createInPushConnectors(config), scope, config));
}

ReaderPtr Factory::createReader(const Scope &scope,
				const ParticipantConfig &config) {
    return ReaderPtr(new Reader(createInPullConnectors(config), scope, config));
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

vector<InPullConnectorPtr>
Factory::createInPullConnectors(const ParticipantConfig &config) {
    // Note: getTransports() only returns *enabled* transports.
    vector<InPullConnectorPtr> connectors;
    set<ParticipantConfig::Transport> configuredTransports = config.getTransports();
    for (set<ParticipantConfig::Transport>::const_iterator transportIt =
             configuredTransports.begin(); transportIt
             != configuredTransports.end(); ++transportIt) {
        RSCDEBUG(logger, "Trying to add connector " << *transportIt);
        Properties options = transportIt->getOptions();
        RSCDEBUG(logger, "Supplied connector options " << transportIt->getOptions());

        // Take care of converters
        if (!options.has("converters")) {
            RSCDEBUG(logger, "Converter configuration for transport `"
                     << transportIt->getName() << "': " << transportIt->getConverters());
            // TODO we should not have to know the transport's wire-type here
            ConverterSelectionStrategy<string>::Ptr converters
                = stringConverterRepository()
                ->getConvertersForDeserialization(pairsToMap<1> (transportIt->getConverters()));
            RSCDEBUG(logger, "Selected converters for transport `"
                     << transportIt->getName() << "': " << converters);
            options["converters"] = converters;
        }
        connectors.push_back(InPullConnectorPtr(InPullFactory::getInstance().createInst(transportIt->getName(), options)));
    }
    return connectors;
}

vector<InPushConnectorPtr>
Factory::createInPushConnectors(const ParticipantConfig &config) {
    // Note: getTransports() only returns *enabled* transports.
    vector<InPushConnectorPtr> connectors;
    set<ParticipantConfig::Transport> configuredTransports = config.getTransports();
    for (set<ParticipantConfig::Transport>::const_iterator transportIt =
             configuredTransports.begin(); transportIt
             != configuredTransports.end(); ++transportIt) {
        RSCDEBUG(logger, "Trying to add connector " << *transportIt);
        Properties options = transportIt->getOptions();
        RSCDEBUG(logger, "Supplied connector options " << transportIt->getOptions());

        // Take care of converters
        if (!options.has("converters")) {
            RSCDEBUG(logger, "Converter configuration for transport `"
                     << transportIt->getName() << "': " << transportIt->getConverters());
            // TODO we should not have to know the transport's wire-type here
            ConverterSelectionStrategy<string>::Ptr converters
                = stringConverterRepository()
                ->getConvertersForDeserialization(pairsToMap<1> (transportIt->getConverters()));
            RSCDEBUG(logger, "Selected converters for transport `"
                     << transportIt->getName() << "': " << converters);
            options["converters"] = converters;
        }
        connectors.push_back(InPushConnectorPtr(InPushFactory::getInstance().createInst(transportIt->getName(), options)));
    }
    return connectors;
}

vector<OutConnectorPtr>
Factory::createOutConnectors(const ParticipantConfig &config) {
    // Note: getTransports() only returns *enabled* transports.
    vector<OutConnectorPtr> connectors;
    set<ParticipantConfig::Transport> configuredTransports = config.getTransports();
    for (set<ParticipantConfig::Transport>::const_iterator transportIt =
             configuredTransports.begin(); transportIt
             != configuredTransports.end(); ++transportIt) {
        RSCDEBUG(logger, "Trying to add connector " << *transportIt);
        Properties options = transportIt->getOptions();
        RSCDEBUG(logger, "Supplied connector options " << transportIt->getOptions());

        // Take care of converters
        if (!options.has("converters")) {
            RSCDEBUG(logger, "Converter configuration for transport `"
                     << transportIt->getName() << "': " << transportIt->getConverters());
            // TODO we should not have to know the transport's wire-type here
            ConverterSelectionStrategy<string>::Ptr converters
                = stringConverterRepository()
                ->getConvertersForSerialization(pairsToMap<2> (transportIt->getConverters()));
            RSCDEBUG(logger, "Selected converters for transport `"
                     << transportIt->getName() << "': " << converters);
            options["converters"] = converters;
        }
        connectors.push_back(OutConnectorPtr(OutFactory::getInstance().createInst(transportIt->getName(), options)));
    }
    return connectors;
}

}
