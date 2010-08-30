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

#ifndef FILTEROBSERVER_H_
#define FILTEROBSERVER_H_

#include <boost/shared_ptr.hpp>

#include "AbstractFilter.h"
#include "ScopeFilter.h"
#include "FilterActionTypes.h"

namespace rsb {

namespace filter {

// TODO Check if Double Dispatch pattern is best suited here
class FilterObserver {
public:
	FilterObserver();
	virtual ~FilterObserver();

	virtual void notify(AbstractFilter* f, FilterAction::Types at);
	virtual void notify(ScopeFilter* f, FilterAction::Types at);
};


typedef boost::shared_ptr<FilterObserver> FilterObserverPtr;


}

}

#endif /* FILTEROBSERVER_H_ */
