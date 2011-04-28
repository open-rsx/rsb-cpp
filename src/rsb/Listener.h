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
#include "Subscription.h"
#include "Handler.h"
#include "eventprocessing/Router.h"
#include "transport/Connector.h"
#include "rsb/rsbexports.h"

namespace rsb {

/**
 * A Listener receives events published by @ref rsb::Informer objects
 * by participating in a channel with a suitable scope. The filters
 * can be added to the associated @ref rsb::Subscription object to
 * reduce the set of events received by a listener. @ref Handler
 * objects have to be added to the listener to actually process
 * received events.
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
     * Constructs a new Listener assigned to the specified
     * scope. The Listener opens a @ref rsb::eventprocessing::Router
     * to ConnectorType Spread and is activated after
     * construction.
     *
     * @param scope the scope where the data is received from.
     */
    Listener(const Scope &scope);

    /**
     * Constructs a new Listener assigned to the specified
     * scope. The Listener opens a @ref rsb::eventprocessing::Router
     * to a ConnectorType specified and is activated after
     * construction.
     *
     * @param scope the scope where the data is received from.
     * @param in the ConnectorType (enum) to connect with, i.e.
     *           transport::Factory::SPREAD
     */
    Listener(transport::Factory::ConnectorTypes in, const Scope &scope);

    virtual ~Listener();

    /**
     * Activates the Listener and therefore the Router. Is considered being in
     * active mode afterwards.
     */
    void activate();

    /**
     * Deactivates the Listener and therefore the Router. Is considered being
     * in passive mode afterwards.
     */
    void deactivate();

    /**
     * Return the associated @ref rsb::Subscription of the
     * listener.
     *
     * @todo the return value should be immutable or a copy
     */
    SubscriptionPtr
    getSubscription();

    /**
     * Replace the associated @ref rsb::Subscription of the listener.
     *
     * @param subscription the new subscription.
     */
    void setSubscription(SubscriptionPtr subscription);

    /**
     * @note modifying the returned set object does not affect the
     * listener's actual set of handlers.
     */
    std::set<HandlerPtr> getHandlers() const;

    /**
     * Appends a @ref rsb::Handler to the Listener. Events which
     * match the restrictions described by the associated
     * @ref rsb::Subscription are passed to all handlers.
     *
     * @param h a Pointer to the Handler.
     */
    virtual void appendHandler(HandlerPtr h);

    void removeHandler(HandlerPtr h);

protected:

    /**
     * Forbidden constructor.
     */
    Listener();

private:
    rsc::logging::LoggerPtr logger;
    Scope scope;
    volatile bool passive;
    SubscriptionPtr subscription;
    std::set<HandlerPtr> handlers;
    eventprocessing::RouterPtr router;

    void initialize(transport::Factory::ConnectorTypes in, const Scope &scope);
};

typedef boost::shared_ptr<Listener> ListenerPtr;

template<typename Ch, typename Tr>
std::basic_ostream<Ch, Tr>&
operator<<(std::basic_ostream<Ch, Tr>& stream, const Listener& listener) {
    stream << "Listener[id=" << listener.getUUID() << "]";
    return stream;
}

}
