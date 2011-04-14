/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "introspection.h"

#include <boost/thread.hpp>

#include "../converter/Repository.h"
#include "IntrospectionConverter.h"

namespace rsb {
namespace introspection {

static bool registered = false;
static boost::mutex registrationMutex;

void registerIntrospectionConverters() {

	boost::mutex::scoped_lock lock(registrationMutex);
	if (!registered) {
		converter::stringConverterRepository()->registerConverter(
				converter::Converter<std::string>::Ptr(
						new IntrospectionConverter));
		registered = true;
	}

}

}
}
