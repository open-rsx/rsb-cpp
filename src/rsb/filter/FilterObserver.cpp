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

FilterObserver::FilterObserver() {
	// TODO Auto-generated constructor stub
}

FilterObserver::~FilterObserver() {
	// TODO Auto-generated destructor stub
}

void FilterObserver::notify(AbstractFilter* f, FilterAction::Types at) {
	cout << "FilterObserver::notify(AbstractFilterPtr a)" << endl;
}

void FilterObserver::notify(ScopeFilter* f, FilterAction::Types at) {
	cout << "FilterObserver::notify(ScopeFilterPtr a)" << endl;
}


}

}
