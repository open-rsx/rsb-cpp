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

#ifndef ABSTRACTFILTER_H_
#define ABSTRACTFILTER_H_

#include "../RSBEvent.h"
#include "FilterActionTypes.h"

namespace rsb {

namespace filter {

class FilterObserver;
typedef boost::shared_ptr<FilterObserver> FilterObserverPtr;

class AbstractFilter {
public:
	AbstractFilter();
	virtual ~AbstractFilter();

	virtual bool match(RSBEventPtr e) = 0;

	virtual void notifyObserver(FilterObserverPtr fo, FilterAction::Types at);
};

typedef boost::shared_ptr<AbstractFilter> AbstractFilterPtr;

}

}

#endif /* ABSTRACTFILTER_H_ */
