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

#include "MethodExistsException.h"

using namespace std;

namespace rsb {
namespace patterns {

MethodExistsException::MethodExistsException(const string& methodName,
		const string& server) :
			Exception(
					"Method '" + methodName + "' already exists for server '"
							+ server + "'") {
}

MethodExistsException::~MethodExistsException() throw () {
}

}
}
