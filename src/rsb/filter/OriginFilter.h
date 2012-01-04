/** ============================================================
 *
 * This file is part of the RSB project.
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * ============================================================  */

#pragma once

#include <rsc/misc/UUID.h>

#include "Filter.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace filter {

/**
 * This filter matches events that originate from a particular
 * participant.
 *
 * @author jmoringe
 */
class RSB_EXPORT OriginFilter: public Filter {
public:
    /**
     * Creates a new origin filter that matches event originating from
     * @a origin.
     *
     * @param origin Id of the participant from which matching events
     *               have to originate.
     * @param invert If true, events match if they do @b not originate
     *               from @a origin.
     */
    OriginFilter(const rsc::misc::UUID& origin,
                 bool                   invert = false);

    rsc::misc::UUID getOrigin() const;

    bool isInverted() const;

    bool match(EventPtr e);

    void notifyObserver(FilterObserverPtr fo, FilterAction::Types at);
private:
    rsc::misc::UUID origin;
    bool            invert;
};

}
}
