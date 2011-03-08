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

const string VoidConverter::TYPE = "void";

VoidConverter::VoidConverter() :
	AbstractConverter<string> (TYPE, TYPE) {
}

VoidConverter::~VoidConverter() {
}

string VoidConverter::serialize(const AnnotatedData &/*data*/, string &wire) {
	wire.clear();
	return TYPE;
}

AnnotatedData VoidConverter::deserialize(const std::string &/*wireType*/,
		const string &/*wire*/) {
	return make_pair(TYPE, boost::shared_ptr<void>());
}

// TODO reenable this
//CREATE_GLOBAL_REGISTREE(stringConverterRegistry(), new VoidConverter, VoidStringConverter)
//;

}

}
