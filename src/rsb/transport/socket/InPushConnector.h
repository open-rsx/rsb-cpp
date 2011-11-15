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

#include <string>

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/runtime/Properties.h>

#include "../../eventprocessing/Handler.h"
#include "../InPushConnector.h"
#include "ConnectorBase.h"
#include "../../Scope.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {
namespace socket {

/**
 * Instances of this class receive events from a bus that is accessed
 * via a socket connection.
 *
 * The receiving and dispatching of events is done in push mode: each
 * instance has a @ref BusConnection which pushes appropriate events
 * into the instance. The connector deserializes event payloads and
 * pushes the events into handlers (usually objects which implement
 * @ref EventReceivingStrategy).
 *
 * @author jmoringe
 */
class RSB_EXPORT InPushConnector: public ConnectorBase,
                                  public transport::InPushConnector,
                                  public eventprocessing::Handler {
public:
    static rsb::transport::InPushConnector* create(const rsc::runtime::Properties& args);

    InPushConnector(ConverterSelectionStrategyPtr  converters,
                    const std::string&             host,
                    unsigned int                   port,
                    bool                           server,
                    bool                           tcpnodelay);
    virtual ~InPushConnector();

    Scope getScope() const;
    void setScope(const Scope& scope);

    void activate();
    void deactivate();

    void setQualityOfServiceSpecs(const QualityOfServiceSpec& specs);

    void handle(EventPtr event);
private:
    rsc::logging::LoggerPtr logger;

    Scope scope;

    volatile bool active;

    void printContents(std::ostream& stream) const;
};

typedef boost::shared_ptr<InPushConnector> InPushConnectorPtr;

}
}
}
