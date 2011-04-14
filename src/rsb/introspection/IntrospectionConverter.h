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

#include "../converter/Converter.h"

#include <string>

namespace rsb {
namespace introspection {

/**
 * @author swrede
 */
class IntrospectionConverter: public rsb::converter::Converter<
		std::string> {
public:
	IntrospectionConverter();
	virtual ~IntrospectionConverter();

	std::string serialize(const rsb::converter::AnnotatedData &data,
			std::string &wire);
	rsb::converter::AnnotatedData deserialize(const std::string &wireType,
			const std::string &wire);

private:
	static const std::string TYPE;

};

typedef boost::shared_ptr<IntrospectionConverter> IntrospectionConverterPtr;

}
}

