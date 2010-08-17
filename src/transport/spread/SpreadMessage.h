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

#ifndef SPREADMESSAGE_H_
#define SPREADMESSAGE_H_

#include <boost/shared_ptr.hpp>
#include <string>
#include <list>

namespace rsb {

namespace spread {

class SpreadMessage {
public:
	enum Type {
		REGULAR = 0x0001, MEMBERSHIP = 0x0002, OTHER = 0xFFFF,
	};
	SpreadMessage() {};
	SpreadMessage(Type mt);
	SpreadMessage(const std::string& d);
	SpreadMessage(const char* d);
	virtual ~SpreadMessage();

	void setData(const std::string& doc);
	void setData(const char* d);
	std::string getDataAsString() const;
	const char* getData() const;
	int getSize() const;
	SpreadMessage::Type getType() const;
	void setType(Type mt);

	void addGroup(const std::string& name);
	int getGroupCount() const;
	std::list<std::string>::const_iterator getGroupsBegin() const;
	std::list<std::string>::const_iterator getGroupsEnd() const;

private:
	std::string data;
	std::list<std::string> groups;
	Type t;
};

typedef boost::shared_ptr<SpreadMessage> SpreadMessagePtr;
}

}

#endif /* SPREADMESSAGE_H_ */
