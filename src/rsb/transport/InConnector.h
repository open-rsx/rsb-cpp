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

#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "../filter/FilterObserver.h"
#include "../eventprocessing/Handler.h"
#include "Connector.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {

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
    virtual void addHandler(eventprocessing::HandlerPtr handler);
    virtual void removeHandler(eventprocessing::HandlerPtr handler);
protected:
    typedef std::list<eventprocessing::HandlerPtr> HandlerList;

    HandlerList handlers;
};

typedef boost::shared_ptr<InConnector> InConnectorPtr;

}
}
