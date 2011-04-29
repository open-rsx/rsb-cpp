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

#include <rsc/runtime/TypeStringTools.h>
#include <rsc/patterns/Singleton.h>

#include "rsb/rsbexports.h"
#include "Informer.h"
#include "Listener.h"
#include "patterns/Server.h"
#include "patterns/RemoteServer.h"
#include "transport/Connector.h"

namespace rsb {

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
     */
    template<class DataType>
    typename Informer<DataType>::Ptr createInformer(
        const Scope &scope,
        const std::string &dataType = rsc::runtime::typeName(typeid(DataType)),
        const std::string &connectorType = "spread") {
        // Create requested connectors
        std::vector<transport::OutConnectorPtr> connectors;
        if (!connectorType.empty()) {
            connectors.push_back(transport::OutConnectorPtr(transport::OutFactory::getInstance().createInst(connectorType)));
        }

        return typename Informer<DataType>::Ptr(new Informer<DataType> (connectors, scope, dataType));
    }

    /**
     * Creates a new listener for the specified scope.
     *
     * @param scope the scope of the new listener
     */
    ListenerPtr createListener(const Scope &scope,
                               const std::string &connectorType = "spread");

    patterns::ServerPtr createServer(const Scope &scope);

    patterns::RemoteServerPtr createRemoteServer(const Scope &scope);

    friend class rsc::patterns::Singleton<Factory>;

private:

    /**
     * Singleton constructor.
     */
    Factory();

};

}
