/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 *               2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "SerializationException.h"

using namespace std;

using namespace boost;

namespace rsb {
namespace converter {

const string Uint64Converter::WIRE_SCHEMA = "uint64";

Uint64Converter::Uint64Converter() :
	Converter<string> (WIRE_SCHEMA, RSB_TYPE_TAG(uint64_t)) {
}

Uint64Converter::~Uint64Converter() {
}

string Uint64Converter::serialize(const AnnotatedData& data, string& wire) {
    assert(data.first == this->getDataType());

    boost::shared_ptr<uint64_t> number = boost::static_pointer_cast<uint64_t>(data.second);
    wire.resize(8);
    for (uint64_t i = 0; i < 8; ++i) {
        wire[i] = (unsigned char) ((*number & (0xffull << (i * 8ull))) >> (i * 8ull));
    }
    return WIRE_SCHEMA;
}

AnnotatedData Uint64Converter::deserialize(const string& wireSchema,
                                           const string& wire) {
    assert(wireSchema == WIRE_SCHEMA);
    assert(wire.size() == 8);

    boost::shared_ptr<uint64_t> number(new uint64_t(0));
    for (uint64_t i = 0; i < 8; ++i) {
        *number |= ((uint64_t) ((unsigned char) wire[i]) << (i * 8ull));
    }
    return make_pair(getDataType(), number);
}

}
}
