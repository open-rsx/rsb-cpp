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

#ifndef UNSUPPORTEDQUALITYOFSERVICEEXCEPTION_H_
#define UNSUPPORTEDQUALITYOFSERVICEEXCEPTION_H_

#include <stdexcept>

#include "QualityOfServiceSpec.h"

namespace rsb {

/**
 * An exception indicating that a desired QualityOfServiceSpec cannot be reached
 * by a transport.
 *
 * @author jwienke
 */
class UnsupportedQualityOfServiceException: std::runtime_error {
public:

	UnsupportedQualityOfServiceException(const std::string &message,
			const QualityOfServiceSpec &spec);
	virtual ~UnsupportedQualityOfServiceException() throw ();

	virtual const char* what() const throw ();

private:

	std::string message;
	QualityOfServiceSpec spec;

};

}

#endif /* UNSUPPORTEDQUALITYOFSERVICEEXCEPTION_H_ */
