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

#include "FilterObserver.h"

#include <iostream>

using namespace std;

namespace rsb {
namespace filter {

FilterObserver::FilterObserver() :
	logger(rsc::logging::Logger::getLogger("rsb.filter.FilterObserver")) {
}

FilterObserver::~FilterObserver() {
}

void FilterObserver::notify(AbstractFilter */*filter*/,
		const FilterAction::Types &/*at*/) {
	RSCDEBUG(logger, "FilterObserver::notify(AbstractFilterPtr a)");
}

void FilterObserver::notify(ScopeFilter */*filter*/,
		const FilterAction::Types &/*at*/) {
	RSCDEBUG(logger, "FilterObserver::notify(ScopeFilterPtr a)");
}

}
}
