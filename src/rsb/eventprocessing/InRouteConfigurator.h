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

#include <string>
#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/runtime/Printable.h>

#include "../QualityOfServiceSpec.h"
#include "../transport/InConnector.h"
#include "EventReceivingStrategy.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace eventprocessing {

/**
 * A class responsible of configuring the route that processes incoming events
 * from one or more InConnector instances in one Listener. This responsibility
 * includes updates to the route from adding or removing Filter or Handler
 * instances.
 *
 * @author swrede
 */
class RSB_EXPORT InRouteConfigurator: public virtual rsc::runtime::Printable,
                                      private boost::noncopyable {
public:
    typedef std::set<transport::InConnectorPtr> ConnectorSet;

    InRouteConfigurator(const Scope &scope);
    virtual ~InRouteConfigurator();

    std::string getClassName() const;
    void printContents(std::ostream &stream) const;

    virtual void activate();
    virtual void deactivate();

    EventReceivingStrategyPtr getEventReceivingStrategy() const;

    ConnectorSet getConnectors();

    void addConnector(transport::InConnectorPtr connector);
    void removeConnector(transport::InConnectorPtr connector);

    void filterAdded(filter::FilterPtr filter);
    void filterRemoved(filter::FilterPtr filter);

    /**
     * Define the desired quality of service specifications for published
     * events.
     *
     * @param specs QoS specification
     * @throw UnsupportedQualityOfServiceException requirements cannot be met
     */
    void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);
private:
    rsc::logging::LoggerPtr logger;
    Scope scope;
    ConnectorSet connectors;
    // ep for observation model
    EventReceivingStrategyPtr eventReceivingStrategy;
    volatile bool shutdown;
    virtual EventReceivingStrategyPtr createEventReceivingStrategy() = 0;
};

typedef boost::shared_ptr<InRouteConfigurator> InRouteConfiguratorPtr;

}
}
