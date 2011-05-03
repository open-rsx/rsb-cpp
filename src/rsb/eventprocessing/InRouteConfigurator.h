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
#include <list>

#include <boost/shared_ptr.hpp>

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
class RSB_EXPORT InRouteConfigurator: public virtual rsc::runtime::Printable {
public:

    InRouteConfigurator(const Scope &scope);
    virtual ~InRouteConfigurator();

    std::string getClassName() const;
    void printContents(std::ostream &stream) const;

    void activate();
    void deactivate();

    void addConnector(transport::InConnectorPtr connector);
    void removeConnector(transport::InConnectorPtr connector);

    /**
     * Adds a new handler that will be notified about received events.
     *
     * @param handler the handler to add
     * @param wait if set to @c true, this call will return only after the
     *             handler has been completely installed and will receive the
     *             next available event
     */
    void handlerAdded(HandlerPtr handler, const bool &wait);

    /**
     * Removes a previously registered handle.
     *
     * @param handler handler to remove
     * @param wait if set to @c true, this call will return only after the
     *             handler has been completely removed and will not be notified
     *             anymore
     */
    void handlerRemoved(HandlerPtr handler, const bool &wait);

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

    /**
     * Sets the desired error strategy to use.
     *
     * @param strategy the strategy to use
     */
    void setErrorStrategy(const ParticipantConfig::ErrorStrategy &strategy);

private:
    typedef std::list<transport::InConnectorPtr> ConnectorList;

    rsc::logging::LoggerPtr logger;
    Scope scope;
    ConnectorList connectors;
    // ep for observation model
    EventReceivingStrategyPtr eventReceivingStrategy;
    ParticipantConfig::ErrorStrategy errorStrategy;
    volatile bool shutdown;

};

typedef boost::shared_ptr<InRouteConfigurator> InRouteConfiguratorPtr;

}
}
