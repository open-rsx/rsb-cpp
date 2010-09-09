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

#include "MatchAndExecute.h"

#include <iostream>

using namespace std;
using namespace rsb;

namespace rsb {

namespace internal {

MatchAndExecute::MatchAndExecute() {
}

MatchAndExecute::~MatchAndExecute() {
	// TODO Auto-generated destructor stub
}

void MatchAndExecute::process(SubscriptionPtr sub, RSBEventPtr e) {
	bool match = false;
	// match event
	try {
		match = sub->match(e);
	} catch (const std::exception& ex) {
		// TODO probably disable this subscription
		cerr << ex.what() << endl;
	} catch (...) {
		cerr << "catchall handler caught exception" << endl;
	}
	// dispatch event
	try {
		// TODO iterate over all actions
		if (match && sub->isEnabled())
			(*sub->getActions()->begin())(e);
	} catch (const std::exception& ex) {
		cerr << ex.what() << endl;
	} catch (...) {
		cerr << "catchall handler caught exception" << endl;
	}
}

}

}
