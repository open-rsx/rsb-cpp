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

#include "ConnectorBase.h"

#include "Bus.h"
#include "Factory.h"

using namespace std;

using namespace rsc::logging;

namespace rsb {
namespace transport {
namespace socket {

ConnectorBase::ConnectorBase(ConverterSelectionStrategyPtr  converters,
                             const string                  &host,
                             unsigned int                   port,
                             bool                           server) :
    ConverterSelectingConnector<string>(converters),
    logger(Logger::getLogger("rsb.transport.socket.ConnectorBase")),
    host(host), port(port), server(server) {
}

ConnectorBase::~ConnectorBase() {
}

void ConnectorBase::activate() {
    RSCDEBUG(logger, "Activating");

    // This connector is added to the connector list of the bus by
    // getBus{Server,Client}For.
    this->bus = (this->server
                 ? Factory::getInstance().getBusServerFor(this->host,
                                                          this->port,
                                                          this)
                 : Factory::getInstance().getBusClientFor(this->host,
                                                          this->port,
                                                          this));

    RSCDEBUG(logger, "Using bus " << getBus());
}

void ConnectorBase::deactivate() {
    RSCDEBUG(logger, "Deactivating");

    RSCDEBUG(logger, "Removing ourselves from connector list of bus " << getBus());
    getBus()->removeConnector(this);
    this->bus.reset();
}

BusPtr ConnectorBase::getBus() {
    return this->bus;
}

}
}
}
