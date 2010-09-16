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

#ifndef INTROSPECTIONCONVERTER_H_
#define INTROSPECTIONCONVERTER_H_

#include "../transport/AbstractConverter.h"

#include <string>

namespace rsb {

namespace introspection {

class IntrospectionConverter: public rsb::transport::AbstractConverter<std::string> {
public:
	IntrospectionConverter();
	virtual ~IntrospectionConverter();

	std::string getTypeName();

	void serialize(const std::string &type, boost::shared_ptr<void> data, std::string &m);
	boost::shared_ptr<void> deserialize(const std::string &type, const std::string &d);
};

typedef boost::shared_ptr<IntrospectionConverter> IntrospectionConverterPtr;

}

}

#endif /* INTROSPECTIONCONVERTER_H_ */
