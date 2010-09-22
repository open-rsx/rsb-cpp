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

#include "VoidConverter.h"

#include <rsc/misc/Registry.h>

using namespace std;

namespace rsb {

namespace transport {

VoidConverter::VoidConverter() {
}

VoidConverter::~VoidConverter() {
}

string VoidConverter::getTypeName() {
	return "void";
}

void VoidConverter::serialize(const std::string &/*type*/,
		boost::shared_ptr<void> /*data*/, string &m) {
	m.clear();
}

boost::shared_ptr<void> VoidConverter::deserialize(const std::string &/*type*/,
		const string &/*d*/) {
	return boost::shared_ptr<void>();
}

// we need this typedef for the macro to work
typedef AbstractConverter<std::string> AbstractStringConverter;
CREATE_GLOBAL_REGISTREE(AbstractStringConverter, VoidConverter)
;

}

}
