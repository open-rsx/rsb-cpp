/*
 * BTL2StringConverter.cpp
 *
 *  Created on: 05.08.2010
 *      Author: swrede
 */

#include "BTL2StringConverter.h"

#include <iostream>

using namespace std;

namespace rsb {

BTL2StringConverter::BTL2StringConverter() {
	// TODO Auto-generated constructor stub

}

BTL2StringConverter::~BTL2StringConverter() {
	// TODO Auto-generated destructor stub
}

void BTL2StringConverter::serialize(std::string type, boost::shared_ptr<void> data, string &m) {
	if (type=="person") {
		PersonPtr p = boost::static_pointer_cast<Person>(data);
		m = p->toXML();
	}
}


boost::shared_ptr<void> BTL2StringConverter::deserialize(std::string type, const string &d) {
	boost::shared_ptr<void> p;
	cout << "Content of d " << d << endl;
	if (type=="person" && d=="<person/>") {
		// would be constructed from d
		// PersonPtr p(parse(d));
		p = boost::static_pointer_cast<void>(PersonPtr(new Person()));
	} else {
		throw("No such type registered at TypeFactory!");
	}
	return p;
};

}
