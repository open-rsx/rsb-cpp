/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include "rsb/rsbexports.h"

namespace rsb {
namespace filter {

/**
 * A class to encapsulate enum constants that specify changes of a Filter for
 * FilterObserver instances.
 *
 * @author swrede
 */
class RSB_EXPORT FilterAction {
public:
    /**
     * Possible actions with filters.
     *
     * @note There is not update action because filters would then need a more
     *       detailed observation model. Instead, the assumption is that a
     *       filter's settings will not change after it has been notified to a
     *       FilterObserver.
     * @author swrede
     */
    enum Types {
        /**
         * A filter shall be added to the FilterObserver.
         */
        ADD,
        /**
         * A filter shall be remove from the observer.
         */
        REMOVE
    };
};

}
}

