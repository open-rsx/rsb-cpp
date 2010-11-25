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

#include "Uint64Converter.h"

#include <rsc/misc/Registry.h>

using namespace std;

namespace rsb {

namespace transport {

Uint64Converter::Uint64Converter() {
}

Uint64Converter::~Uint64Converter() {
}

string Uint64Converter::getTypeName() {
	return "uint64";
}

void Uint64Converter::serialize(const std::string &type,
		boost::shared_ptr<void> data, string &m) {
	// TODO common check for all converters, refactor to base class
	if (type == getTypeName()) {
		boost::shared_ptr<boost::uint64_t> number = boost::static_pointer_cast<
				boost::uint64_t>(data);
		stringstream s;
		s << *number;
		m = s.str();
	}
}

boost::shared_ptr<void> Uint64Converter::deserialize(const std::string &type,
		const string &d) {
	boost::shared_ptr<void> p;
	// TODO this check will be a common case. Refactor it to the base class
	if (type == getTypeName()) {
		stringstream s;
		s << d;
		boost::uint64_t number;
		// TODO check fail bit
		s >> number;
		return boost::shared_ptr<uint64_t>(new boost::uint64_t(number));
	} else {
		// TODO better exception required
		throw("No such type registered at TypeFactory!");
	}
	return p;
}

CREATE_GLOBAL_REGISTREE(stringConverterRegistry(), new Uint64Converter, Uint64StringConverter)
;

}

}
