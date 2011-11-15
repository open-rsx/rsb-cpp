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

#include <boost/format.hpp>

#include "Bus.h"
#include "Factory.h"

using namespace std;

using namespace boost;

using namespace rsc::logging;

namespace rsb {
namespace transport {
namespace socket {

ConnectorBase::ConnectorBase(ConverterSelectionStrategyPtr converters,
                             const string&                 host,
                             unsigned int                  port,
                             Server                        server,
                             bool                          tcpnodelay) :
    ConverterSelectingConnector<string>(converters),
    logger(Logger::getLogger("rsb.transport.socket.ConnectorBase")),
    host(host), port(port), server(server), tcpnodelay(tcpnodelay) {
}

ConnectorBase::~ConnectorBase() {
}

void ConnectorBase::activate() {
    RSCDEBUG(logger, "Activating");

    // This connector is added to the connector list of the bus by
    // getBus{Server,Client}For.
    RSCINFO(logger, "Server mode: " << this->server);
    Factory& factory = Factory::getInstance();
    switch (this->server) {
    case SERVER_NO:
        this->bus = factory.getBusClientFor(this->host,
                                            this->port,
                                            this->tcpnodelay,
                                            this);
        break;
    case SERVER_YES:
        this->bus = factory.getBusServerFor(this->host,
                                            this->port,
                                            this->tcpnodelay,
                                            this);
        break;
    case SERVER_AUTO:
        try {
            this->bus = factory.getBusServerFor(this->host,
                                                this->port,
                                                this->tcpnodelay,
                                                this);
        } catch (const std::exception& e) {
            RSCINFO(logger, "Could not create server for bus: " << e.what()
                    << "; trying to access bus as client");
            this->bus = factory.getBusClientFor(this->host,
                                                this->port,
                                                this->tcpnodelay,
                                                this);
        }
        break;
    }

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
