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

#ifndef STRINGCONVERTER_H_
#define STRINGCONVERTER_H_

#include "../AbstractConverter.h"

#include <string>
#include <boost/shared_ptr.hpp>

namespace rsb {
namespace transport {

// converts any string into any string serializing content to a string...
class StringConverter: public rsb::transport::AbstractConverter<std::string> {
public:
	StringConverter();
	virtual ~StringConverter();

	std::string getTypeName();

	void serialize(const std::string &type, boost::shared_ptr<void> data, std::string &m);
	boost::shared_ptr<void> deserialize(const std::string &type, const std::string &d);
};

typedef boost::shared_ptr<StringConverter> StringConverterPtr;

}
}

#endif /* STRINGCONVERTER_H_ */
