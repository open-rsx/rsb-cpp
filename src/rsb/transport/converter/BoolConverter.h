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

#ifndef BOOLCONVERTER_H_
#define BOOLCONVERTER_H_

#include "../AbstractConverter.h"

#include <string>
#include <boost/shared_ptr.hpp>

namespace rsb {
namespace transport {

/**
 * Converter for bool.
 *
 * @author jwienke
 */
class BoolConverter: public rsb::transport::AbstractConverter<std::string> {
public:

	BoolConverter();
	virtual ~BoolConverter();

	std::string serialize(const AnnotatedData &data, std::string &wire);
	AnnotatedData deserialize(const std::string &wireType,
			const std::string &wire);

private:
	static const std::string WIRE_TYPE;

};

typedef boost::shared_ptr<BoolConverter> BoolConverterPtr;

}
}

#endif /* STRINGCONVERTER_H_ */
