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

#include <vector>

#include "Filter.h"
#include "FilterObserver.h"
#include "FilterActionTypes.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace filter {

/**
 * @author swrede
 */
class RSB_EXPORT FilterObservable {
public:
	FilterObservable();
	virtual ~FilterObservable();

	void notifyObservers(FilterPtr af, FilterAction::Types at);

	void addObserver(FilterObserverPtr observer);

	void removeObserver(FilterObserverPtr observer);

	void clearObservers();

private:
	boost::shared_ptr< std::vector<FilterObserverPtr> > observers;
};

}

}

