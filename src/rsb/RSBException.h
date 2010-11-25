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

#ifndef RSBEXCEPTION_H_
#define RSBEXCEPTION_H_

#include <exception>
#include <string>

#include "rsb/rsbexports.h"

namespace rsb {

/**
 * @author swrede
 * @todo remove header implementation
 * @todo implement stream operators to add variables to exception message
 */
class RSB_EXPORT RSBException: public std::exception {
public:
	RSBException();
	RSBException(const std::string& m) :
		msg(m) {
	}
	virtual ~RSBException() throw () {
	}
	const char* what() const throw () {
		return msg.c_str();
	}
private:
	const std::string msg;
};

}

#endif /* RSBEXCEPTION_H_ */

