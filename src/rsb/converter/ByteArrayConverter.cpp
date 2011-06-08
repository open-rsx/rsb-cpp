/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "ByteArrayConverter.h"

#include <rsc/misc/Registry.h>

using namespace std;

namespace rsb {
namespace converter {

const string ByteArrayConverter::WIRE_SCHEMA = ".*";

ByteArrayConverter::ByteArrayConverter() :
    Converter<string> (WIRE_SCHEMA, "bytearray") {
}

ByteArrayConverter::~ByteArrayConverter() {
}

string ByteArrayConverter::serialize(const AnnotatedData &/*data*/, string &/*wire*/) {
    throw runtime_error("This converter should only be used for *de*serialization.");
}

AnnotatedData ByteArrayConverter::deserialize(const std::string &/*wireType*/,
		const string &wire) {
    return make_pair(getDataType(), boost::shared_ptr<string>(new string(wire.c_str(), wire.length())));
}

}
}
