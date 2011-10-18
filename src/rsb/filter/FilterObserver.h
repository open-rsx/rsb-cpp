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

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/runtime/Printable.h>

#include "FilterActionTypes.h"

namespace rsb {
namespace filter {

class Filter;
class ScopeFilter;

/**
 * @author swrede
 * @todo Check if Double Dispatch pattern is best suited here
 */
class RSB_EXPORT FilterObserver: public virtual rsc::runtime::Printable {
public:
	FilterObserver();
	virtual ~FilterObserver();

	virtual void notify(Filter *filter, const FilterAction::Types &at);
	virtual void notify(ScopeFilter *filter, const FilterAction::Types &at);

private:

	rsc::logging::LoggerPtr logger;

};

typedef boost::shared_ptr<FilterObserver> FilterObserverPtr;

}

}

