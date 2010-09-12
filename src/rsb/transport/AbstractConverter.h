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

#ifndef ABSTRACTCONVERTER_H_
#define ABSTRACTCONVERTER_H_

#include <string>
#include <rsb/RSBEvent.h>

namespace rsb {

namespace transport {

// T is the serialization format, uchar, string, binary, ...
template < class T >
class AbstractConverter {
public:
	AbstractConverter() {};
	virtual ~AbstractConverter() {};

	virtual void serialize(const std::string &type, boost::shared_ptr<void> d, T &m) = 0;
	virtual boost::shared_ptr<void> deserialize(const std::string &type, const T &m) = 0;
};

}

}

#endif /* ABSTRACTCONVERTER_H_ */
