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

#ifndef UINT64CONVERTER_H_
#define UINT64CONVERTER_H_

#include "../AbstractConverter.h"

#include <string>
#include <boost/shared_ptr.hpp>

namespace rsb {
namespace transport {

/**
 * Converter for uint64 types.
 *
 * @author jwienke
 */
class Uint64Converter: public rsb::transport::AbstractConverter<std::string> {
public:
	Uint64Converter();
	virtual ~Uint64Converter();

	std::string serialize(const AnnotatedData &data, std::string &wire);
	AnnotatedData deserialize(const std::string &wireType,
			const std::string &wire);

private:

	static const std::string TYPE;

};

typedef boost::shared_ptr<Uint64Converter> Uint64ConverterPtr;

}
}

#endif /* UINT64CONVERTER_H_ */
