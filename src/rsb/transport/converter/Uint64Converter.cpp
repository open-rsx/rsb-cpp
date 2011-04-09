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

#include <sstream>

#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>

#include "../SerializationException.h"

using namespace std;

namespace rsb {
namespace transport {

const string Uint64Converter::WIRE_SCHEMA = "uint64";

Uint64Converter::Uint64Converter() :
			AbstractConverter<string> (WIRE_SCHEMA,
					reinterpret_cast<boost::uint64_t*> (0)) {
}

Uint64Converter::~Uint64Converter() {
}

string Uint64Converter::serialize(const AnnotatedData &data, string &wire) {
	assert(data.first == this->getDataType());

	boost::shared_ptr<boost::uint64_t> number = boost::static_pointer_cast<
			boost::uint64_t>(data.second);
	stringstream s;
	s << *number;
	wire = s.str();
	return WIRE_SCHEMA;
}

AnnotatedData Uint64Converter::deserialize(const string &wireSchema,
		const string &wire) {
	assert(wireSchema == WIRE_SCHEMA);

	try {
		return make_pair(
				getDataType(),
				boost::shared_ptr<boost::uint64_t>(
						new boost::uint64_t(
								boost::lexical_cast<boost::uint64_t>(wire))));
	} catch (boost::bad_lexical_cast &e) {
		throw SerializationException(
				string("Unable to cast wire contents to number: ") + e.what());
	}

}

}
}
