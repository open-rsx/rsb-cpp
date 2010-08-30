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

#ifndef UUID_H_
#define UUID_H_

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/shared_ptr.hpp>

namespace rsb {

namespace util {

class UUID {
public:
	UUID();
	UUID(std::string uuid);
	virtual ~UUID();

	boost::uuids::uuid getId() const {
		return id;
	}

	std::string getIdAsString() const;

private:
	boost::uuids::uuid id;
	 // TODO refactor to singleton
	static boost::uuids::basic_random_generator<boost::mt19937> gen;
};

typedef boost::shared_ptr<UUID> UUIDPtr;

}

}

#endif /* UUID_H_ */
