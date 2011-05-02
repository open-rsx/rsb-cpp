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

#include "../Event.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace eventprocessing {

/** Implementations of this class can be used in contexts where an
 * "event sink" is required.
 *
 * @author jmoringe
 */
class RSB_EXPORT Handler {
public:
    virtual ~Handler();

    /** Handle @a event.
     *
     * @param event The event that should be handled.
     */
    virtual void handle(EventPtr event) = 0;
};

typedef boost::shared_ptr<Handler> HandlerPtr;

}

}
