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

#ifndef FILTEROBSERVABLE_H_
#define FILTEROBSERVABLE_H_

#include "AbstractFilter.h"
#include "FilterObserver.h"
#include "FilterActionTypes.h"

#include <vector>

namespace rsb {

namespace filter {

class FilterObservable {
public:
	FilterObservable();
	virtual ~FilterObservable();

	void notifyObservers(AbstractFilterPtr af, FilterAction::Types at);

	void addObserver(FilterObserverPtr observer);

	void removeObserver(FilterObserverPtr observer);

	void clearObservers();

private:
	boost::shared_ptr< std::vector<FilterObserverPtr> > observers;
};

}

}

#endif /* FILTEROBSERVABLE_H_ */
