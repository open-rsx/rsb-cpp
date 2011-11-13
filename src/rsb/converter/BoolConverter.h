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

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

#include "Converter.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace converter {

/**
 * Converter for bool.
 *
 * @author jwienke
 */
class RSB_EXPORT BoolConverter: public Converter<std::string> {
public:

	BoolConverter();
	virtual ~BoolConverter();

	std::string serialize(const AnnotatedData& data, std::string& wire);
	AnnotatedData deserialize(const std::string& wireSchema,
			const std::string& wire);

private:
	static const std::string WIRE_SCHEMA;

};

typedef boost::shared_ptr<BoolConverter> BoolConverterPtr;

}
}
