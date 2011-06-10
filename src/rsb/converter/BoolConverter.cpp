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

#include <sstream>

#include "SerializationException.h"

using namespace std;

namespace rsb {
namespace converter {

const string BoolConverter::WIRE_SCHEMA = "bool";

BoolConverter::BoolConverter() :
	Converter<string> (WIRE_SCHEMA, RSB_TYPE_TAG(bool)) {
}

BoolConverter::~BoolConverter() {
}

string BoolConverter::serialize(const AnnotatedData &data, string &wire) {
	assert(data.first == this->getDataType());

	boost::shared_ptr<bool> s = boost::static_pointer_cast<bool>(data.second);
	if (*s) {
		wire = "t";
	} else {
		wire.clear();
	}
	return WIRE_SCHEMA;
}

AnnotatedData BoolConverter::deserialize(const std::string &wireSchema,
		const string &wire) {
	assert(wireSchema == WIRE_SCHEMA);

	if (wire.empty()) {
		return make_pair(getDataType(),
				boost::shared_ptr<bool>(new bool(false)));
	} else {
		return make_pair(getDataType(), boost::shared_ptr<bool>(new bool(true)));
	}
}

}
}
