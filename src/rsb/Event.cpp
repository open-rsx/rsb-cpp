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

#include "EventId.h"
#include "MetaData.h"
#include "Scope.h"

using namespace std;
using namespace boost;

namespace rsb {

class Event::Impl {
public:
    EventIdPtr id;
    ScopePtr scope;

    VoidPtr content;

    // is this a single type, a hierarchy or a set?
    std::string type;

    std::string method;

    std::set<EventId> causes;

    MetaData metaData;
};

Event::Event() :
    d(new Impl()) {
    d->scope.reset(new Scope);
}

Event::Event(const Event &event) :
    d(new Impl(*event.d)) {

}

Event::Event(ScopePtr scope, boost::shared_ptr<void> payload,
        const string &type, const string &method) :
    d(new Impl()) {
    d->scope = scope;
    d->content = payload;
    d->type = type;
    d->method = method;
}

Event::Event(Scope scope, boost::shared_ptr<void> payload, const string &type,
        const string &method) :
    d(new Impl()) {
    d->scope.reset(new Scope(scope));
    d->content = payload;
    d->type = type;
    d->method = method;
}

Event::~Event() {
}

string Event::getClassName() const {
    return "Event";
}

void Event::printContents(ostream &stream) const {
    stream << "id = ";
    if (d->id) {
        stream << d->id;
    } else {
        stream << "UNSPECIFIED";
    }
    stream << ", type = " << d->type << ", scope = ";
    if (d->scope) {
        stream << *d->scope;
    } else {
        stream << "UNSPECIFIED";
    }
    stream << ", metaData = " << d->metaData << ", method = " << d->method;
    stream << ", causes = " << d->causes;
}

boost::uint64_t Event::getSequenceNumber() const {
    return getEventId().getSequenceNumber();
}

rsc::misc::UUID Event::getId() const {
    return getEventId().getAsUUID();
}

EventId Event::getEventId() const {
    if (!d->id) {
        throw rsc::misc::IllegalStateException(
                "The event does not contain id information.");
    }
    return *d->id;
}

void Event::setEventId(const rsc::misc::UUID &senderId,
        const boost::uint32_t &sequenceNumber) {
    d->id.reset(new EventId(senderId, sequenceNumber));
    d->metaData.setSenderId(senderId);
}

void Event::setScopePtr(ScopePtr s) {
    d->scope = s;
}

void Event::setScope(const Scope &s) {
    d->scope = ScopePtr(new Scope(s));
}

ScopePtr Event::getScopePtr() const {
    return d->scope;
}

Scope Event::getScope() const {
    return *d->scope;
}

void Event::setData(VoidPtr data) {
    d->content = data;
}

VoidPtr Event::getData() {
    return d->content;
}

string Event::getType() const {
    return d->type;
}

void Event::setType(const string &t) {
    d->type = t;
}

bool Event::addCause(const EventId &id) {
    return d->causes.insert(id).second;
}

bool Event::removeCause(const EventId &id) {
    return d->causes.erase(id) > 0;
}

bool Event::isCause(const EventId &id) const {
    return d->causes.count(id) > 0;
}

set<EventId> Event::getCauses() const {
    return d->causes;
}

string Event::getMethod() const {
    return d->method;
}

void Event::setMethod(const string &method) {
    d->method = method;
}

MetaData Event::getMetaData() const {
    return d->metaData;
}

MetaData &Event::mutableMetaData() {
    return d->metaData;
}

void Event::setMetaData(const MetaData &metaData) {
    d->metaData = metaData;
}

}
