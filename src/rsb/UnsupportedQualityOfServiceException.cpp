/* ============================================================
 *
 * This file is a part of RSB project
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

#include "UnsupportedQualityOfServiceException.h"

#include <sstream>

using namespace std;

namespace rsb {

UnsupportedQualityOfServiceException::UnsupportedQualityOfServiceException(
		const std::string &message, const QualityOfServiceSpec &spec) :
	Exception(message), message(message), spec(spec) {
}

UnsupportedQualityOfServiceException::~UnsupportedQualityOfServiceException() throw () {
}

const char* UnsupportedQualityOfServiceException::what() const throw () {
	stringstream s;
	s << message << ": specs: " << spec;
	return s.str().c_str();
}

}
