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

#include <rsc/runtime/Printable.h>

#include "../Event.h"
#include "FilterActionTypes.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace filter {

class FilterObserver;
typedef boost::shared_ptr<FilterObserver> FilterObserverPtr;

/**
 * @author swrede
 */
class RSB_EXPORT Filter: public virtual rsc::runtime::Printable {
public:
	Filter();
	virtual ~Filter();

	virtual bool match(EventPtr e) = 0;

	virtual void notifyObserver(FilterObserverPtr fo, FilterAction::Types at);
};

typedef boost::shared_ptr<Filter> FilterPtr;

}
}

