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

#include <rsc/logging/Logger.h>

#include <boost/shared_ptr.hpp>

#include "Event.h"
#include "Participant.h"
#include "Handler.h"
#include "eventprocessing/InRouteConfigurator.h"
#include "transport/Connector.h"
#include "rsb/rsbexports.h"

namespace rsb {

/**
 * A Listener receives events published by @ref rsb::Informer objects
 * by participating in a channel with a suitable scope. @ref Handler
 * objects have to be added to the listener to actually process
 * received events. These events can be filtered for all handlers by adding
 * Filter instances to this class. Filter form a conjunction.
 *
 * Usage example:
 * @code
 * ListenerPtr s(new Listener("rsb://example/informer"));
 * boost::shared_ptr<MyDataHandler> dh(new MyDataHandler());
 * s->appendHandler(dh);
 * @endcode
 *
 * @author swrede
 *
 * @todo how to deal with IDs of listeners? (it must be possible to reference them internally somehow
 * @todo use templates in subscriptions only? (however, they need the event info)
 */
class RSB_EXPORT Listener: public Participant {
public:

    /**
     * Constructs a new Listener assigned to the specified scope. The
     * Listener connects to the bus using the supplied connectors.
     *
     * @param connectors a list of connectors that the listener should
     *                   use to communicate with the bus.
     * @param scope the scope where the data is received from.
     * @param config the configuration that was used to setup this listener
     */
    Listener(const std::vector<transport::InConnectorPtr> &connectors,
            const Scope &scope, const ParticipantConfig &config);

    virtual ~Listener();

    std::string getClassName() const;

    /**
     * Adds a filter that will be applied after some time (but not immediately
     * after this call) for all handlers.
     *
     * @param filter filter to add
     */
    void addFilter(filter::FilterPtr filter);

    /**
     * Removes a previously installed filter if it is present by pointer
     * comparison some time after this call.
     *
     * @param filter filter to remove if present
     */
    void removeFilter(filter::FilterPtr filter);

    /**
     * @note modifying the returned set object does not affect the
     * listener's actual set of handlers.
     */
    std::set<HandlerPtr> getHandlers() const;

    /**
     * Adds a @ref rsb::Handler to the Listener. Events which
     * match the restrictions described by the associated
     * filters are passed to all handlers.
     *
     * @param h a Pointer to the Handler.
     * @param wait if set to @c true, this method will return only after the
     *             handler has completely been installed and will receive the
     *             next available message. Otherwise it may return earlier.
     */
    virtual void addHandler(HandlerPtr h, const bool &wait = true);

    /**
     * Removes a Handler instance to process newly received events.
     *
     * @param h handler to remove if present (comparison based on pointer)
     * @param wait if set to @c true, this method will return only after the
     *             handler has been completely removed from the event processing
     *             and will not be called anymore from this listener
     */
    void removeHandler(HandlerPtr h, const bool &wait = true);

private:
    rsc::logging::LoggerPtr logger;
    std::set<HandlerPtr> handlers;
    eventprocessing::InRouteConfiguratorPtr configurator;

    void initialize(const std::vector<transport::InConnectorPtr> &connectors,
            const Scope &scope);
};

typedef boost::shared_ptr<Listener> ListenerPtr;

}
