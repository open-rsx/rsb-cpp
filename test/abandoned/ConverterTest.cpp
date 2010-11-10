/*
 * TransportFactoryTest.cpp
 *
 *  Created on: 03.08.2010
 *      Author: swrede
 */

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <boost/shared_ptr.hpp>

#include "rsb/transport/AbstractConverter.h"
#include "BTL2StringConverter.h"

using namespace std;
using namespace rsb;
using namespace rsb::transport;

typedef boost::shared_ptr<AbstractConverter <string> > StringConverterPtr;

int main(void) {
	StringConverterPtr c(new BTL2StringConverter());
	PersonPtr p1(new Person());
	boost::shared_ptr<void> p_u = boost::static_pointer_cast<void>(p1);
	string s;
	c->serialize("Person",p_u,s);
	cout << "Result of serialize call: " << s << endl;
	PersonPtr p2 = boost::static_pointer_cast<Person>(c->deserialize("Person",s));
	cout << "Result of deserialize call: " << p2->toXML() << endl;
}
