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

const uint16_t ConnectorBase::DEFAULT_PORT = 55555;

ConnectorBase::ConnectorBase(ConverterSelectionStrategyPtr converters,
                             const string&                 host,
                             unsigned int                  port,
                             const string&                 server,
                             bool                          tcpnodelay) :
    ConverterSelectingConnector<string>(converters),
    logger(Logger::getLogger("rsb.transport.socket.ConnectorBase")),
    host(host), port(port), tcpnodelay(tcpnodelay) {
    if (server == "0") {
        this->server = SERVER_NO;
    } else if (server == "1") {
        this->server = SERVER_YES;
    } else if (server == "auto") {
        this->server = SERVER_AUTO;
    } else {
        throw invalid_argument(str(format("Invalid server/client specification: %1%")
                                   % server));
    }
}

ConnectorBase::~ConnectorBase() {
}

void ConnectorBase::activate() {
    RSCDEBUG(logger, "Activating");

    // This connector is added to the connector list of the bus by
    // getBus{Server,Client}For.
    Factory& factory = Factory::getInstance();
    switch (this->server) {
    case SERVER_YES:
        this->bus = factory.getBusClientFor(this->host,
                                            this->port,
                                            this->tcpnodelay,
                                            this);
        break;
    case SERVER_NO:
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
