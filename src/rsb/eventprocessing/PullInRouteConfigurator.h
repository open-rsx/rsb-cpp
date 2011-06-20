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

#include "InRouteConfigurator.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace eventprocessing {

/**
 * Objects of this @ref InRouteConfigurator class setup and maintain
 * the required components for a pull-style event receiving
 * configuration. In particular, a @ref PullEventReceivingStrategy is
 * instanciated to retrieve events from connectors.
 *
 * @author jmoringe
 */
class RSB_EXPORT PullInRouteConfigurator: public InRouteConfigurator {
public:
    PullInRouteConfigurator(const Scope &scope);

    /**
     * Create and return a @ref PullEventReceivingStrategy .
     *
     * @return The create @ref PullEventReceivingStrategy .
     */
    EventReceivingStrategyPtr createEventReceivingStrategy();
private:
    std::string getClassName() const;
};

}
}
