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

#include <map>
#include <list>

#include <boost/thread/recursive_mutex.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/patterns/Singleton.h>

#include "../../Event.h"
#include "../../Scope.h"
#include "InConnector.h"

namespace rsb {
namespace inprocess {

/**
 *
 * @author jmoringe
 */
class Bus : public rsc::patterns::Singleton<Bus> {
public:
    Bus();
    virtual ~Bus();

    void addSink(InConnectorPtr sink);
    void removeSink(InConnector* sink);

    void push(EventPtr event);
private:
    typedef std::list< boost::weak_ptr<InConnector> > SinkList;
    typedef std::map<Scope, SinkList> SinkMap;

    rsc::logging::LoggerPtr logger;

    SinkMap sinks;
    boost::recursive_mutex mutex;
};

}
}
