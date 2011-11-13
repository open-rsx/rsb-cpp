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

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>

#include "../ConverterSelectingConnector.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {
namespace socket {

// Forward declaration
class Bus;
typedef boost::shared_ptr<Bus> BusPtr;

/**
 * This class is intended to be used as a base class for connector
 * classes of the socket-based transport.
 *
 * It provides establishing and managing a connection to the bus via
 * an instance of the @ref Bus class.
 *
 * @author jmoringe
 */
class RSB_EXPORT ConnectorBase: public ConverterSelectingConnector<std::string> {
public:
    ConnectorBase(ConverterSelectionStrategyPtr  converters,
                  const std::string&             host,
                  unsigned int                   port,
                  bool                           server);

    virtual ~ConnectorBase();
protected:
    void activate();

    void deactivate();

    /**
     * Return the @ref Bus object through which this connector is
     * connected to a socket-based bus.
     *
     * @return a shared_pointer to the bus object.
     */
    BusPtr getBus();
private:
    rsc::logging::LoggerPtr logger;

    BusPtr                  bus;

    std::string             host;
    unsigned int            port;
    bool                    server;
};

typedef boost::shared_ptr<ConnectorBase> ConnectorBasePtr;

}
}
}
