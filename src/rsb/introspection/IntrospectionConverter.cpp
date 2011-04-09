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

#include "IntrospectionConverter.h"

#include "PortStateChange.h"
#include "../protocol/ProtocolException.h"

using namespace std;
using namespace rsb::protocol;
using namespace rsb::protocol::introspection;

namespace rsb {
namespace introspection {

const string IntrospectionConverter::TYPE = "portstatechange";

IntrospectionConverter::IntrospectionConverter() :
	rsb::transport::AbstractConverter<string>(TYPE, TYPE) {

}

IntrospectionConverter::~IntrospectionConverter() {
}

string IntrospectionConverter::serialize(
		const rsb::transport::AnnotatedData &data, string &wire) {
	assert(data.first == TYPE);
	PortStateChangePtr psc = boost::static_pointer_cast<PortStateChange>(
			data.second);
	// extremely simple here as so far directly the pbuf objects are used as domain objects
	if (!psc->SerializeToString(&wire)) {
		// TODO this does not match the normal converter exception specification. what to do?
		throw ProtocolException("Failed to write notification to stream");
	}
	return TYPE;
}

rsb::transport::AnnotatedData IntrospectionConverter::deserialize(
		const std::string &wireType, const string &wire) {
	assert(wireType == TYPE);

	PortStateChangePtr psc(new PortStateChange());
	if (!psc->ParseFromString(wire)) {
		// TODO yet another wrong exception. What to do here?
		throw CommException("Failed to parse notification in pbuf format");
	}

	return make_pair(TYPE, psc);

}

}
}
