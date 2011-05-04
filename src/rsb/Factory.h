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
#include <boost/thread/recursive_mutex.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/runtime/TypeStringTools.h>
#include <rsc/patterns/Singleton.h>

#include "rsb/rsbexports.h"
#include "Informer.h"
#include "Listener.h"
#include "patterns/Server.h"
#include "patterns/RemoteServer.h"
#include "transport/Connector.h"
#include "ParticipantConfig.h"
#include "Service.h"

namespace rsb {

template <unsigned int which, typename C>
std::map<typename C::value_type::first_type,
         typename C::value_type::second_type>
pairsToMap(const C &container) {
    typedef typename C::value_type::first_type first_type;
    typedef typename C::value_type::second_type second_type;

    typedef typename C::const_iterator const_iterator;

    std::map<first_type, second_type> result;
    for (const_iterator it = container.begin();
         it != container.end(); ++it) {
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
     * @param dataType the string representation of the data type used to select
     *                 converters
     * @return new informer instance
     */
    template<class DataType>
    typename Informer<DataType>::Ptr createInformer(
            const Scope &scope,
            const ParticipantConfig &config =
                    Factory::getInstance().getDefaultParticipantConfig(),
            const std::string &dataType = rsc::runtime::typeName(
                    typeid(DataType))) {
        // Create requested connectors
        std::vector<transport::OutConnectorPtr> connectors;
        std::set<ParticipantConfig::Transport> configuredTransports =
                config.getTransports();
        for (std::set<ParticipantConfig::Transport>::const_iterator
                 transportIt = configuredTransports.begin(); transportIt
                 != configuredTransports.end(); ++transportIt) {
            RSCDEBUG(logger, "Trying to add connector " << *transportIt);
            rsc::runtime::Properties options = transportIt->getOptions();
            if (!transportIt->getConverters().empty()) {
                options["converters"] = pairsToMap<2>(transportIt->getConverters());
            }
            connectors.push_back(
                    transport::OutConnectorPtr(
                            transport::OutFactory::getInstance().createInst(
                                    transportIt->getName(), options)));
        }

        return typename Informer<DataType>::Ptr(
                new Informer<DataType> (connectors, scope, defaultConfig,
                        dataType));
    }

    /**
     * Creates a new listener for the specified scope.
     *
     * @param scope the scope of the new listener
     * @return new listener instance
     */
    ListenerPtr createListener(
            const Scope &scope,
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
     * Singleton constructor.
     */
    Factory();

    rsc::logging::LoggerPtr logger;

    /**
     * Always acquire configMutex before reading or writing the config.
     */
    ParticipantConfig defaultConfig;
    mutable boost::recursive_mutex configMutex;

};

}
