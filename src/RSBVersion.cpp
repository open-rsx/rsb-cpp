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

#include <sstream>
#include "RSBVersion.h"

using namespace std;

namespace rsb {

	std::string RSB_VERSION_STR() {
		 ostringstream ss;
		 ss << MAJOR_VERSION << '.' << MINOR_VERSION << " (ABI " << ABI_VERSON << ")";
		 return ss.str();
	}

	std::string RSB_BUILD_STR() {
		 ostringstream ss;
		 ss << BUILD_NUMBER << " (DATE " << BUILD_DATE << ")";
		 return ss.str();
	}

}
