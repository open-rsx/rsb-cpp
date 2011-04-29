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

#include <rsc/misc/UUID.h>

#include "rsb/rsbexports.h"
#include "Scope.h"

namespace rsb {

/**
 * Objects of this class participate in the exchange of notifications
 * on one channel of the bus. Since each channel is identified by
 * scope, each @ref rsb::Participant has an associated scope. On the
 * other hand, scopes do not uniquely identify @ref rsb::Participant s
 * since zero, one or more of these can participate in the exchange of
 * any given scope. For this reason, each @ref rsb::Participant has a
 * unique id.
 *
 *  @author jmoringe
 */
class RSB_EXPORT Participant {
public:

    /**
     * Returns the unique id of the participant.
     *
     * @return unique id
     */
    rsc::misc::UUID getId() const;

    /**
     * Returns the scope of this participant.
     *
     * @return scope
     */
    Scope getScope() const;

protected:

    Participant(const Scope &scope);

private:

    rsc::misc::UUID id;
    Scope scope;

};

}
