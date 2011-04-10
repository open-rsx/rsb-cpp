/*
 * BTL2StringConverter.h
 *
 *  Created on: 05.08.2010
 *      Author: swrede
 */

#pragma once

#include "rsb/transport/AbstractConverter.h"

#include <boost/shared_ptr.hpp>
#include <string>

namespace rsb {

class Person {
public:
	std::string toXML() {
		return "<person/>";
	}
};

typedef boost::shared_ptr<Person> PersonPtr;

class BTL2StringConverter : public rsb::transport::AbstractConverter<std::string> {
public:
	BTL2StringConverter();
	virtual ~BTL2StringConverter();

	std::string getTypeName();

	void serialize(const std::string &type, boost::shared_ptr<void> data, std::string &m);
	boost::shared_ptr<void> deserialize(const std::string &type, const std::string &d);
};

typedef boost::shared_ptr<BTL2StringConverter> BTL2StringConverterPtr;

}

