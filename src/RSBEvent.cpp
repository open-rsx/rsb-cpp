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

#include "RSBEvent.h"
#include <ostream>

using namespace std;
using namespace rsb::util;

namespace rsb {

RSBEvent::~RSBEvent() {
	// TODO Auto-generated destructor stub
}

UUID RSBEvent::getUUID() {
	return id;
}

void RSBEvent::setURI(std::string s) {
	uri = s;
}

std::string RSBEvent::getURI() {
	return uri;
}

void RSBEvent::setData(VoidPtr d) {
	content = d;
}

VoidPtr RSBEvent::getData() {
	return content;
}

std::string RSBEvent::getType() {
	return type;
}

void RSBEvent::setType(std::string t) {
	type = t;
}

ostream& operator<<(ostream& out, const RSBEvent& e)
{
	//out.precision(3);
	out << "RSBEvent[id=" << e.id.getIdAsString() << " type=" << e.type <<
						  " uri=" << e.uri <<"] ";
	return out;
}

}
