/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
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

Event::Event(const Event& event) :
    d(new Impl(*event.d)) {

}

Event::Event(ScopePtr scope, boost::shared_ptr<void> payload,
        const string& type, const string& method) :
    d(new Impl()) {
    d->scope = scope;
    d->content = payload;
    d->type = type;
    d->method = method;
}

Event::Event(Scope scope, boost::shared_ptr<void> payload, const string& type,
        const string& method) :
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

void Event::printContents(ostream& stream) const {
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
    return getId().getSequenceNumber();
}

EventId Event::getId() const {
    if (!d->id) {
        throw rsc::misc::IllegalStateException(
                "The event does not contain id information.");
    }
    return *d->id;
}

EventId Event::getEventId() const {
    return getId();
}

void Event::setId(const rsc::misc::UUID& senderId,
        const boost::uint32_t& sequenceNumber) {
    d->id.reset(new EventId(senderId, sequenceNumber));
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    d->metaData.setSenderId(senderId);
#pragma GCC diagnostic pop
}

void Event::setEventId(const rsc::misc::UUID& senderId,
        const boost::uint32_t& sequenceNumber) {
    setId(senderId, sequenceNumber);
}

void Event::setScopePtr(ScopePtr s) {
    d->scope = s;
}

void Event::setScope(const Scope& s) {
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

void Event::setType(const string& t) {
    d->type = t;
}

bool Event::addCause(const EventId& id) {
    return d->causes.insert(id).second;
}

bool Event::removeCause(const EventId& id) {
    return d->causes.erase(id) > 0;
}

bool Event::isCause(const EventId& id) const {
    return this->d->causes.find(id) != this->d->causes.end();
}

set<EventId> Event::getCauses() const {
    return d->causes;
}

string Event::getMethod() const {
    return d->method;
}

void Event::setMethod(const string& method) {
    d->method = method;
}

MetaData Event::getMetaData() const {
    return d->metaData;
}

MetaData& Event::mutableMetaData() {
    return d->metaData;
}

void Event::setMetaData(const MetaData& metaData) {
    d->metaData = metaData;
}

}
