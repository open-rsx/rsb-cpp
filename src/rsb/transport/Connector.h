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
#include <boost/enable_shared_from_this.hpp>

#include "../Event.h"
#include "../Handler.h"
#include "../filter/FilterObserver.h"
#include "../QualityOfServiceSpec.h"
#include "../eventprocessing/Handler.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {

/**
 * @author swrede
 */
class RSB_EXPORT Connector {
public:
    virtual ~Connector();

    virtual void activate() = 0;
    virtual void deactivate() = 0;

    /**
     * Requests new QoS settings for publishing events. Does not
     * influence the receiving part.
     *
     * @param specs QoS specification
     * @throw UnsupportedQualityOfServiceException requirements cannot be met
     */
    virtual void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs) = 0;
};

typedef boost::shared_ptr<Connector> ConnectorPtr;

/** Objects of classes which implement this interface can be used to
 * receive events by means of one transport mechanism.
 *
 * Received events are dispatched to an associated observer.
 *
 * @author jmoringe
 */
class RSB_EXPORT InConnector : public Connector,
                               public rsb::filter::FilterObserver,
                               public boost::enable_shared_from_this<InConnector> {
public:
    virtual void addHandler(HandlerPtr handler);
    virtual void removeHandler(HandlerPtr handler);
protected:
    typedef std::list<eventprocessing::HandlerPtr> HandlerList;

    HandlerList handlers;
};

typedef boost::shared_ptr<InConnector> InConnectorPtr;

/** Objects of classes which implement this interface can be used to
 * send events by means of one transport mechanism.
 *
 * @author jmoringe
 */
class RSB_EXPORT OutConnector : public Connector,
                                public eventprocessing::Handler {
public:
};

typedef boost::shared_ptr<OutConnector> OutConnectorPtr;

}
}
