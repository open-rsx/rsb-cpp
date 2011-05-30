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

#include <rsc/runtime/ContainerIO.h>

using namespace std;

namespace rsb {

Event::Event() {
}

Event::Event(const Scope &scope, boost::shared_ptr<void> payload,
        const string &type, const string &method) :
    scope(scope), content(payload), type(type), method(method) {
}

Event::~Event() {
}

string Event::getClassName() const {
    return "Event";
}

void Event::printContents(ostream &stream) const {
    stream << "id = " << id.getIdAsString() << " type = " << type
            << " scope = " << scope << " meta-infos = " << this->metaInfos
            << ", metaData = " << metaData;
}

void Event::setId(const rsc::misc::UUID &id) {
    this->id = id;
}

rsc::misc::UUID Event::getId() {
    return id;
}

void Event::setScope(const Scope &s) {
    scope = s;
}

Scope Event::getScope() {
    return scope;
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

void Event::setType(const string &t) {
    type = t;
}

string Event::getMethod() const {
    return method;
}

void Event::setMethod(const string &method) {
    this->method = method;
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

void Event::addMetaInfo(const string &key, const string &value, bool override) {

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

MetaData Event::getMetaData() const {
    return metaData;
}

MetaData &Event::mutableMetaData() {
    return metaData;
}

void Event::setMetaData(const MetaData &metaData) {
    this->metaData = metaData;
}

}
