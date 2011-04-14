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

#include <rsc/misc/Registry.h>

using namespace std;

namespace rsb {
namespace transport {

const string StringConverter::WIRE_SCHEMA = "string";

StringConverter::StringConverter() :
	Converter<string> (WIRE_SCHEMA, RSB_TYPE_TAG(string)) {
}

StringConverter::~StringConverter() {
}

string StringConverter::serialize(const AnnotatedData &data, string &wire) {
	assert(data.first == this->getDataType());

	boost::shared_ptr<string> s = boost::static_pointer_cast<string>(
			data.second);
	// essentially return the contained string to the serialization medium
	wire = *s;
	return WIRE_SCHEMA;
}

AnnotatedData StringConverter::deserialize(const std::string &wireType,
		const string &wire) {
	assert(wireType == WIRE_SCHEMA);

	return make_pair(getDataType(), boost::shared_ptr<string>(new string(wire)));
}

}
}
