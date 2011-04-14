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

#include "Event.h"

#include <ostream>

using namespace std;

namespace rsb {

Event::Event() {
}

Event::Event(const string &uri, boost::shared_ptr<void> payload,
		const string &type) :
	uri(uri), content(payload), type(type) {
}

Event::~Event() {
}

void Event::setUUID(const rsc::misc::UUID &id) {
	this->id = id;
}

rsc::misc::UUID Event::getUUID() {
	return id;
}

void Event::setURI(string s) {
	uri = s;
}

string Event::getURI() {
	return uri;
}

void Event::setData(VoidPtr d) {
	content = d;
}

VoidPtr Event::getData() {
	return content;
}

string Event::getType() {
	return type;
}

void Event::setType(string t) {
	type = t;
}

bool Event::hasMetaInfo(const string &key) const {
	return metaInfos.count(key);
}

string Event::getMetaInfo(const string &key) const {
	if (metaInfos.count(key)) {
		return metaInfos.find(key)->second;
	} else {
		throw runtime_error("No meta info registered under key '" + key + "'");
	}
}

void Event::addMetaInfo(const string &key, const string &value,
		bool override) {

	if (metaInfos.count(key) && !override) {
		throw runtime_error(
				"There already is meta info registered under key '" + key + "'");
	}
	metaInfos[key] = value;

}

map<string, string>::const_iterator Event::metaInfoBegin() const {
	return metaInfos.begin();
}

map<string, string>::const_iterator Event::metaInfoEnd() const {
	return metaInfos.end();
}

ostream& operator<<(ostream& out, const Event& e) {
	//out.precision(3);
	out << "Event[id=" << e.id.getIdAsString() << " type=" << e.type
			<< " uri=" << e.uri << "] ";
	return out;
}

}
