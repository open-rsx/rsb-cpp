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

#include "UUID.h"

#include <sstream>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>

using namespace std;

namespace rsb {

namespace util {

boost::uuids::basic_random_generator<boost::mt19937> UUID::gen = boost::uuids::basic_random_generator<boost::mt19937>();

UUID::UUID() {
	id = gen();

}

UUID::UUID(string uuid) {
	if (uuid != "") {
		boost::uuids::string_generator gen;
		id = gen(uuid);
	}
}

string UUID::getIdAsString() const {
	std::stringstream ss;
	ss << id;
	return ss.str();
}

UUID::~UUID() {
	// TODO Auto-generated destructor stub
}

}

}
