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

#include "StringConverter.h"

using namespace std;

namespace rsb {

namespace transport {

StringConverter::StringConverter() {
	// TODO Auto-generated constructor stub

}

StringConverter::~StringConverter() {
	// TODO Auto-generated destructor stub
}

void StringConverter::serialize(std::string type, boost::shared_ptr<void> data, string &m) {
	if (type=="string") {
		boost::shared_ptr<string> s = boost::static_pointer_cast<string>(data);
		// essentially return the contained string to the serialization medium
		m = *s;
	}
}


boost::shared_ptr<void> StringConverter::deserialize(std::string type, const string &d) {
	boost::shared_ptr<void> p;
	if (type=="portstatechange") {
		boost::shared_ptr<string> s(new string(d));
		p = boost::static_pointer_cast<void>(s);
	} else {
		throw("No such type registered at TypeFactory!");
	}
	return p;
};

}

}
