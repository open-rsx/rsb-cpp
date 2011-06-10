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

#pragma once

#include <list>

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/runtime/Printable.h>

#include "../QualityOfServiceSpec.h"
#include "../transport/Connector.h"
#include "EventSendingStrategy.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace eventprocessing {

/**
 *
 *
 * @author swrede
 * @todo add configuration, provide preliminary set up interface
 */
class RSB_EXPORT OutRouteConfigurator: public virtual rsc::runtime::Printable {
public:
    OutRouteConfigurator();
    virtual ~OutRouteConfigurator();

    std::string getClassName() const;
    void printContents(std::ostream &stream) const;

    void activate();
    void deactivate();

    void addConnector(transport::OutConnectorPtr connector);
    void removeConnector(transport::OutConnectorPtr connector);

    /**
     * Publish event to out ports of this router.
     *
     * @param e event to publish
     */
    void publish(EventPtr e);

    /**
     * Define the desired quality of service specifications for published
     * events.
     *
     * @param specs QoS specification
     * @throw UnsupportedQualityOfServiceException requirements cannot be met
     */
    void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);
private:
    typedef std::list<transport::OutConnectorPtr> ConnectorList;

    rsc::logging::LoggerPtr logger;

    ConnectorList connectors;
    // ep for observation model
    EventSendingStrategyPtr eventSendingStrategy;
    volatile bool shutdown;
};

typedef boost::shared_ptr<OutRouteConfigurator> OutRouteConfiguratorPtr;

}
}
