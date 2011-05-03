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

#include "../transport/Connector.h"
#include "EventSendingStrategy.h"

namespace rsb {
namespace eventprocessing {

/** This event sending strategy just passes incoming events to its
 * associated @ref rsb::transport::OutConnector s without
 * modification, queueing or anything else.
 *
 * @author jmoringe
 */
class DirectEventSendingStrategy: public EventSendingStrategy {
public:

    std::string getClassName() const;
    void printContents(std::ostream &stream) const;

    void addConnector(transport::OutConnectorPtr connector);
    void removeConnector(transport::OutConnectorPtr connector);

    void process(EventPtr e);
private:
    typedef std::list<transport::OutConnectorPtr> ConnectorList;

    ConnectorList connectors;
};

}
}
