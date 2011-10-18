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

#include "OriginFilter.h"

#include "FilterObserver.h"
#include "../MetaData.h"

using namespace rsc::misc;

namespace rsb {
namespace filter {

OriginFilter::OriginFilter(const UUID &origin,
                           bool        invert):
    origin(origin), invert(invert) {
}

UUID OriginFilter::getOrigin() const {
    return this->origin;
}

bool OriginFilter::isInverted() const {
    return this->invert;
}

bool OriginFilter::match(EventPtr e) {
    bool result = this->origin == e->getMetaData().getSenderId();
    return this->invert ? !result : result;
}

void OriginFilter::notifyObserver(FilterObserverPtr   fo,
                                  FilterAction::Types at) {
    fo->notify(this, at);
}

}
}
