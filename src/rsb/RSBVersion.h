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

#ifndef RSBVERSION_H_
#define RSBVERSION_H_

#include <string>

#include "rsb/rsbexports.h"

namespace rsb {

#define MAJOR_VERSION 1
#define MINOR_VERSION 0
#define ABI_VERSON 1
#define BUILD_NUMBER 1
#define BUILD_DATE "28/07/2010"

RSB_EXPORT std::string RSB_VERSION_STR();
RSB_EXPORT std::string RSB_BUILD_STR();

}

#endif /* RSBVERSION_H_ */

