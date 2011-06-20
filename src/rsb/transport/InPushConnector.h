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

#include "../eventprocessing/Handler.h"
#include "InConnector.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {

/**
 * Objects of classes which implement this specialized @ref
 * InConnector interface provide the ability to receive events in
 * push-style manner by means of one transport mechanism.
 *
 * @author jmoringe
 */
class RSB_EXPORT InPushConnector: public InConnector {
public:
    typedef boost::shared_ptr<InPushConnector> Ptr;

    virtual ~InPushConnector();

    virtual void addHandler(eventprocessing::HandlerPtr handler);
    virtual void removeHandler(eventprocessing::HandlerPtr handler);
protected:
    typedef std::list<eventprocessing::HandlerPtr> HandlerList;

    HandlerList handlers;
};

typedef InPushConnector::Ptr InPushConnectorPtr;

}
}
