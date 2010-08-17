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

#ifndef RSBEVENT_H_
#define RSBEVENT_H_

#include "util/UUID.h"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

// base / interface class for all events
// data is set via static_cast:
// RSBEventPtr p(RSBEvent(boost::static_pointer_cast<void> boost::shared_ptr<Person>(new Person())));

namespace rsb {

typedef boost::shared_ptr<void> VoidPtr;

class RSBEvent {
public:
	RSBEvent() {};
	RSBEvent(std::string u, boost::shared_ptr<void> c, std::string t) :
		uri(u), content(c), type(t) {
	};

	virtual ~RSBEvent();

	rsb::util::UUID getUUID();

	// TODO REFACTOR: introduce domain object URI
	std::string getURI();
	void setURI(std::string);

	// TODO REFACTOR: introduce domain object for type (hierarchy?)
	std::string getType();
	void setType(std::string);

	VoidPtr getData();
	void setData(VoidPtr d);

	friend std::ostream& operator<< (std::ostream& out, const RSBEvent& e);

private:
	rsb::util::UUID id;
	std::string uri;

	VoidPtr content;

	// is this a single type, a hierarchy or a set?
	std::string type;
};



typedef boost::shared_ptr<RSBEvent> RSBEventPtr;


}

#endif /* RSBEVENT_H_ */
