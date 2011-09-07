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

#include <rsc/runtime/Properties.h>

#include <rsc/logging/Logger.h>

#include "../OutConnector.h"

#include "ConnectorBase.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {
namespace socket {

/**
 * Instances of this connector class push events into @ref Bus objects
 * which take care of socket-based broadcast and delivery.
 *
 * @author jmoringe
 */
class RSB_EXPORT OutConnector: public ConnectorBase,
                               public rsb::transport::OutConnector {
public:
    OutConnector(ConverterSelectionStrategyPtr  converters,
                 const std::string             &host,
                 unsigned int                   port,
                 bool                           server);

    void activate();
    void deactivate();

    void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);

    void handle(EventPtr e);

    static transport::OutConnector *create(const rsc::runtime::Properties &args);
private:
    rsc::logging::LoggerPtr logger;
};

}
}
}
