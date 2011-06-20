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

#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/runtime/TypeStringTools.h>
#include <rsc/patterns/Singleton.h>

#include "rsb/rsbexports.h"
#include "Informer.h"
#include "Listener.h"
#include "patterns/Server.h"
#include "patterns/RemoteServer.h"
#include "converter/UnambiguousConverterMap.h"
#include "converter/Repository.h"
#include "transport/Connector.h"
#include "ParticipantConfig.h"
#include "Service.h"

namespace rsb {

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

/**
 * Factory for RSB user-level domain objects for communication patterns.
 *
 * @author jwienke
 */
class RSB_EXPORT Factory: public rsc::patterns::Singleton<Factory> {
public:

    virtual ~Factory();

    /**
     * Creates a new informer in the specified scope.
     *
     * @tparam DataType the C++ data type this informer publishes
     * @param scope the scope of the informer
     * @param config the configuration for the informer to use, defaults to the
     *               general config held in this factory.
     * @param dataType the string representation of the data type used to select
     *                 converters
     * @return new informer instance
     */
    template<class DataType>
    typename Informer<DataType>::Ptr createInformer(const Scope &scope,
            const ParticipantConfig &config =
                    Factory::getInstance().getDefaultParticipantConfig(),
            const std::string &dataType = rsc::runtime::typeName(
                    typeid(DataType))) {
        // Create requested connectors
        std::vector<transport::OutConnectorPtr> connectors
            = createConnectors<transport::OutFactory>(config);
        return typename Informer<DataType>::Ptr(new Informer<DataType> (
                connectors, scope, config, dataType));
    }

    /**
     * Creates a new listener for the specified scope.
     *
     * @param scope the scope of the new listener
     * @param config the configuration for the LISTENER to use, defaults to the
     *               general config held in this factory.f
     * @return new listener instance
     */
    ListenerPtr createListener(const Scope &scope,
            const ParticipantConfig &config =
                    Factory::getInstance().getDefaultParticipantConfig());

    /**
     * Creates a Service instance operating on the given scope.
     *
     * @param scope parent-scope of the new service
     * @return new service instance
     */
    ServicePtr createService(const Scope &scope);

    patterns::ServerPtr createServer(const Scope &scope);

    patterns::RemoteServerPtr createRemoteServer(const Scope &scope);

    /**
     * Returns the default configuration for new participants.
     *
     * @return copy of the default configuration
     */
    ParticipantConfig getDefaultParticipantConfig() const;

    /**
     * Sets the default config for participants that will be used for every new
     * participant that is created after this call.
     *
     * @param config new config
     */
    void setDefaultParticipantConfig(const ParticipantConfig &config);

    friend class rsc::patterns::Singleton<Factory>;

private:

    /**
     * This function is only required to have the same factory instance on
     * windows because there the Singleton template is instantiated once per
     * compilation unit. For the template-based createInformer method this will
     * then be in the caller's compilation unit. :/
     */
    transport::OutFactory &getOutFactoryInstance();

    /**
     * Singleton constructor.
     */
    Factory();

    rsc::logging::LoggerPtr logger;

    /**
     * Always acquire configMutex before reading or writing the config.
     */
    ParticipantConfig defaultConfig;
    mutable boost::recursive_mutex configMutex;

    template <typename Factory>
    std::vector<typename Factory::InterfaceType::Ptr> createConnectors(const ParticipantConfig &config) {
        typedef typename Factory::InterfaceType::Ptr ConnectorPtr;
        // Create requested connectors
        std::vector<ConnectorPtr> connectors;
        std::set<ParticipantConfig::Transport> configuredTransports = config.getTransports();
        for (std::set<ParticipantConfig::Transport>::const_iterator transportIt =
                 configuredTransports.begin(); transportIt
                 != configuredTransports.end(); ++transportIt) {
            RSCDEBUG(logger, "Trying to add connector " << *transportIt);
            rsc::runtime::Properties options = transportIt->getOptions();
            RSCDEBUG(logger, "Supplied connector options " << transportIt->getOptions());

            // Take care of converters
            if (!options.has("converters")) {
                RSCDEBUG(logger, "Converter configuration for transport `"
                         << transportIt->getName() << "': " << transportIt->getConverters());
                // TODO we should not have to know the transport's wire-type here
                converter::ConverterSelectionStrategy<std::string>::Ptr converters;
                if (boost::is_same<Factory, transport::InPushFactory>::value
                    || boost::is_same<Factory, transport::InPullFactory>::value) {
                    converters = converter::stringConverterRepository()
		      ->getConvertersForDeserialization(pairsToMap<1> (transportIt->getConverters()));
                } else {
		    converters = converter::stringConverterRepository()
			->getConvertersForSerialization(pairsToMap<2> (transportIt->getConverters()));
                }
                RSCDEBUG(logger, "Selected converters for transport `"
                         << transportIt->getName() << "': " << converters);
                options["converters"] = converters;
            }
            connectors.push_back(ConnectorPtr(
                                     Factory::getInstance().createInst(
                                         transportIt->getName(), options)));
        }
        return connectors;
    }

};

}
