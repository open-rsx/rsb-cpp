/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "BoolConverter.h"

#include <rsc/misc/Registry.h>

using namespace std;

namespace rsb {

namespace transport {

BoolConverter::BoolConverter() {
}

BoolConverter::~BoolConverter() {
}

string BoolConverter::getTypeName() {
	return "bool";
}

void BoolConverter::serialize(const std::string &type,
		boost::shared_ptr<void> data, string &m) {
	// TODO common check for all converters, refactor to base class
	if (type == getTypeName()) {
		boost::shared_ptr<bool> s = boost::static_pointer_cast<bool>(data);
		if (*s) {
			m = "t";
		} else {
			m.clear();
		}
	}
}

boost::shared_ptr<void> BoolConverter::deserialize(const std::string &type,
		const string &d) {
	boost::shared_ptr<void> p;
	// TODO this check will be a common case. Refactor it to the base class
	if (type == getTypeName()) {
		if (d.empty()) {
			return boost::shared_ptr<bool>(new bool(false));
		} else {
			return boost::shared_ptr<bool>(new bool(true));
		}
	} else {
		// TODO better exception required
		throw("No such type registered at TypeFactory!");
	}
	return p;
}

CREATE_GLOBAL_REGISTREE(stringConverterRegistry(), new BoolConverter, BoolStringConverter);
;

}

}
