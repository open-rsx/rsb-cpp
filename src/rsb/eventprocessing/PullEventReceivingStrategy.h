/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <set>

#include <boost/shared_ptr.hpp>

#include "EventReceivingStrategy.h"

#include "../filter/Filter.h"

#include "../transport/InPullConnector.h"

namespace rsb {
namespace eventprocessing {

/**
 * Instances of this class retrieve @ref Event s from @ref
 * transport::Connector s when explicitly asked by a client (which
 * usually is a @ref Participant ). The Retrival works (roughly) as
 * follows:
 *
 * -# The client calls the emit method of the @c
 *    PullEventReceivingStrategy instance
 * -# The instance calls the emit methods of its connectors
 * -# A connector calls the handle method of the instance
 * -# The instance performs event filtering
 * -# If the event has not been discarded during filtering, it is
 *    returned from emit
 *
 * @author jmoringe
 */
class RSB_EXPORT PullEventReceivingStrategy: public EventReceivingStrategy {
public:
    PullEventReceivingStrategy(const std::set<transport::InPullConnectorPtr> &connectors);

    virtual void addFilter(filter::FilterPtr filter);
    virtual void removeFilter(filter::FilterPtr filter);

    /**
     * Retrieve an @ref Event from the connectors and return it.
     * @param block Controls whether the call should block until an
     * event becomes available if that is not the case immediately.
     *
     * @return A pointer to the received event.
     */
    EventPtr emit(bool block);

    /**
     * This method is called by @ref rsb::transport::InPullConnector s
     * in response to calls to their emit methods.
     *
     * @param event The @ref Event that should be filtered and
     * potentially be delivered to the client.
     */
    void handle(EventPtr event);
private:
    typedef std::set<transport::InPullConnectorPtr> ConnectorSet;
    typedef std::set<filter::FilterPtr> FilterSet;
    FilterSet filters;

    ConnectorSet connectors;
    EventPtr currentEvent; // stores event obtained via connector callbacks

    std::string getClassName() const;
    void printContents(std::ostream &stream) const;
};

typedef boost::shared_ptr<PullEventReceivingStrategy> PullEventReceivingStrategyPtr;

}
}
