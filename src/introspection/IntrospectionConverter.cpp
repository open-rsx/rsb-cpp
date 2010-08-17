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

IntrospectionConverter::IntrospectionConverter() {
	// TODO Auto-generated constructor stub

}

IntrospectionConverter::~IntrospectionConverter() {
	// TODO Auto-generated destructor stub
}

void IntrospectionConverter::serialize(std::string type, boost::shared_ptr<void> data, string &m) {
	if (type=="portstatechange") {
		PortStateChangePtr psc = boost::static_pointer_cast<PortStateChange>(data);
		// extremely simple here as so far directly the pbuf objects are used as domain objects
		if (!psc->SerializeToString(&m)) {
			throw ProtocolException("Failed to write notification to stream");
		}
	}
}


boost::shared_ptr<void> IntrospectionConverter::deserialize(std::string type, const string &d) {
	boost::shared_ptr<void> p;
	if (type=="portstatechange") {
		PortStateChangePtr psc(new PortStateChange());
		if (!psc->ParseFromString(d)) {
					throw CommException("Failed to parse notification in pbuf format");
		}
		p = boost::static_pointer_cast<void>(psc);
	} else {
		throw("No such type registered at TypeFactory!");
	}
	return p;
};

}

}
