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

#pragma once

#include "../AbstractConverter.h"

#include <string>
#include <boost/shared_ptr.hpp>

namespace rsb {
namespace transport {

/**
 * Converts any string into any string serializing content to a string.
 *
 * @author swrede
 */
class StringConverter: public rsb::transport::AbstractConverter<std::string> {
public:

	StringConverter();
	virtual ~StringConverter();

	std::string serialize(const AnnotatedData &data, std::string &wire);
	AnnotatedData deserialize(const std::string &wireSchema,
			const std::string &wire);

private:
	static const std::string WIRE_SCHEMA;

};

}
}
