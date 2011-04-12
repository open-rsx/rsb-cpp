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

#pragma once

#include "../RSBException.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace patterns {

/**
 * Indicates that a method of a Server already exists.
 *
 * @author jwienke
 */
class RSB_EXPORT MethodExistsException: public rsb::RSBException {
public:

	/**
	 * Constructs a new exception.
	 *
	 * @param methodName name of the method
	 * @param server name of the server
	 */
	MethodExistsException(const std::string &methodName,
			const std::string &server);
	virtual ~MethodExistsException() throw ();
};

}
}

