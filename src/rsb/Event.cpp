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

#include <boost/format.hpp>

#include <rsc/runtime/ContainerIO.h>
#include <rsc/misc/IllegalStateException.h>

using namespace std;
using namespace boost;

namespace rsb {

Event::Event() : scope(new Scope()) {
}

Event::Event(ScopePtr scope, boost::shared_ptr<void> payload,
        const string &type, const string &method) :
        scope(scope), content(payload), type(type), method(method) {
}

Event::Event(Scope scope, boost::shared_ptr<void> payload, const string &type,
        const string &method) :
        scope(new Scope(scope)), content(payload), type(type), method(
                method) {
}

Event::~Event() {
}

string Event::getClassName() const {
    return "Event";
}

void Event::printContents(ostream &stream) const {
    stream << "id = ";
    if (id) {
        stream << id;
    } else {
        stream << "UNSPECIFIED";
    }
    stream << ", type = " << type << ", scope = ";
    if (scope) {
        stream << *scope;
    } else {
        stream << "UNSPECIFIED";
    }
    stream << ", metaData = " << metaData << ", method = " << method;
}

boost::uint64_t Event::getSequenceNumber() const {
    return getEventId().getSequenceNumber();
}

rsc::misc::UUID Event::getId() const {
    return getEventId().getAsUUID();
}

EventId Event::getEventId() const {
    if (!id) {
        throw rsc::misc::IllegalStateException(
                "The event does not contain id information.");
    }
    return *id;
}

void Event::setEventId(const rsc::misc::UUID &senderId,
        const boost::uint32_t &sequenceNumber) {
    id.reset(new EventId(senderId, sequenceNumber));
    metaData.setSenderId(senderId);
}

void Event::setScopePtr(ScopePtr s) {
    this->scope = s;
}

void Event::setScope(const Scope &s) {
    this->scope = ScopePtr(new Scope(s));
}

ScopePtr Event::getScopePtr() const {
    return this->scope;
}

Scope Event::getScope() const {
    return *scope;
}

void Event::setData(VoidPtr d) {
    content = d;
}

VoidPtr Event::getData() {
    return this->content;
}

string Event::getType() const {
    return type;
}

void Event::setType(const string &t) {
    type = t;
}

bool Event::addCause(const EventId &id) {
    return causes.insert(id).second;
}

bool Event::removeCause(const EventId &id) {
    return causes.erase(id) > 0;
}

bool Event::isCause(const EventId &id) const {
    return causes.count(id) > 0;
}

set<EventId> Event::getCauses() const {
    return causes;
}

string Event::getMethod() const {
    return method;
}

void Event::setMethod(const string &method) {
    this->method = method;
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
