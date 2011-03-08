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

#ifndef VOIDCONVERTER_H_
#define VOIDCONVERTER_H_

#include "../AbstractConverter.h"

#include <boost/shared_ptr.hpp>

namespace rsb {
namespace transport {

/**
 * Fulfill the interface... Converts nothing to nothing. ;)
 *
 * @author jwienke
 */
class VoidConverter: public rsb::transport::AbstractConverter<std::string> {
public:
	VoidConverter();
	virtual ~VoidConverter();

	std::string serialize(const AnnotatedData &data, std::string &wire);
	AnnotatedData deserialize(const std::string &wireType,
			const std::string &wire);

private:
	static const std::string TYPE;

};

typedef boost::shared_ptr<VoidConverter> VoidConverterPtr;

}
}

#endif /* VOIDCONVERTER_H_ */
