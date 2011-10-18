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

#include "DirectEventSendingStrategy.h"

#include <rsc/runtime/ContainerIO.h>

#include "../transport/OutConnector.h"

using namespace std;

namespace rsb {
namespace eventprocessing {

string DirectEventSendingStrategy::getClassName() const {
    return "DirectEventSendingStrategy";
}

void DirectEventSendingStrategy::printContents(ostream &stream) const {
    stream << "connectors = " << connectors;
}

void DirectEventSendingStrategy::addConnector(transport::OutConnectorPtr connector) {
    this->connectors.push_back(connector);
}

void DirectEventSendingStrategy::removeConnector(transport::OutConnectorPtr connector) {
    this->connectors.remove(connector);
}

void DirectEventSendingStrategy::process(EventPtr event) {
    for (ConnectorList::const_iterator it = this->connectors.begin();
         it != this->connectors.end(); ++it) {
        (*it)->handle(event);
    }
}

}
}
