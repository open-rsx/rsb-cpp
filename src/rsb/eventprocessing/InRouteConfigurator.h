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

#include "../Subscription.h"
#include "../QualityOfServiceSpec.h"
#include "../transport/Connector.h"
#include "EventReceivingStrategy.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace eventprocessing {

/**
 *
 *
 * @author swrede
 * @todo add configuration, provide preliminary set up interface
 * @todo implement abstract factory pattern for different port types
 * @todo think about null objects for ports to avoid checks for existence
 */
class RSB_EXPORT InRouteConfigurator {
public:
    InRouteConfigurator();
    virtual ~InRouteConfigurator();

    void activate();
    void deactivate();

    void addConnector(transport::InConnectorPtr connector);
    void removeConnector(transport::InConnectorPtr connector);

    /**
     * Add a subscription.
     *
     * @param s subscription to add
     * @param handlers associated handlers
     */
    void subscribe(rsb::SubscriptionPtr s,
                   std::set<HandlerPtr> handlers);

    /**
     * Unsubscribe a subscription.
     *
     * @param s subscription to remove
     */
    void unsubscribe(rsb::SubscriptionPtr s);

    /**
     * Define the desired quality of service specifications for published
     * events.
     *
     * @param specs QoS specification
     * @throw UnsupportedQualityOfServiceException requirements cannot be met
     */
    void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);
protected:
    /**
     * Helper for port notification about subscription status changes.
     */
    void
        notifyConnectors(rsb::SubscriptionPtr s,
                         rsb::filter::FilterAction::Types a);

private:
    typedef std::list<transport::InConnectorPtr> ConnectorList;

    rsc::logging::LoggerPtr logger;
    ConnectorList connectors;
    // ep for observation model
    EventReceivingStrategyPtr eventReceivingStrategy;
    volatile bool shutdown;

};

typedef boost::shared_ptr<InRouteConfigurator> InRouteConfiguratorPtr;

}
}
