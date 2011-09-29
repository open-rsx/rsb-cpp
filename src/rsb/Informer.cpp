/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 *               2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "Informer.h"

#include <boost/format.hpp>

using namespace std;

namespace rsb {

InformerBase::InformerBase(
        const vector<transport::OutConnectorPtr> &connectors,
        const Scope &scope, const ParticipantConfig &config,
        const string &defaultType) :
    Participant(scope, config), defaultType(defaultType),
            currentSequenceNumber(0) {
    // TODO evaluate configuration
    this->configurator.reset(new eventprocessing::OutRouteConfigurator());
    for (vector<transport::OutConnectorPtr>::const_iterator it =
            connectors.begin(); it != connectors.end(); ++it) {
        this->configurator->addConnector(*it);
    }

    this->configurator->activate();
}

InformerBase::~InformerBase() {
}

void InformerBase::printContents(ostream &stream) const {
    Participant::printContents(stream);
    stream << ", type = " << defaultType;
}

string InformerBase::getType() const {
    return this->defaultType;
}

void InformerBase::setQualityOfSerivceSpecs(const QualityOfServiceSpec &specs) {
    configurator->setQualityOfServiceSpecs(specs);
}

EventPtr InformerBase::createEvent() const {
    EventPtr event(new Event());
    event->setScopePtr(getScope());
    return event;
}

EventPtr InformerBase::publish(VoidPtr data, const std::string &type) {
    EventPtr event = createEvent();
    event->setData(data);
    event->setType(type);
    checkedPublish(event);
    return event;
}

EventPtr InformerBase::uncheckedPublish(VoidPtr data, const std::string &type) {
    EventPtr event = createEvent();
    event->setData(data);
    event->setType(type);
    uncheckedPublish(event);
    return event;
}

EventPtr InformerBase::publish(EventPtr event) {
    checkedPublish(event);
    return event;
}

void InformerBase::checkedPublish(EventPtr event) {
    if (event->getType().empty()) {
        throw invalid_argument(
                boost::str(
                        boost::format("Event type cannot be empty: %1%")
                                % event));
    }
    // Check event type against informer's declared type.
    if (!getType().empty() && event->getType() != getType()) {
        throw invalid_argument(
                boost::str(
                        boost::format(
                                "Specified event type %1% does not match listener type %2%.")
                                % event->getType() % getType()));
    }
    // Check event scope against informer's declared scope.
    if (*event->getScopePtr() != *getScope() && !event->getScopePtr()->isSubScopeOf(
            *getScope())) {
        throw invalid_argument(
                boost::str(
                        boost::format(
                                "Specified event scope %1% does not match listener scope %2%.")
                                % event->getScopePtr() % getScope()));
    }

    this->uncheckedPublish(event);
}

void InformerBase::uncheckedPublish(EventPtr event) {
    event->setEventId(getId(), nextSequenceNumber());
    configurator->publish(event);
}

boost::uint32_t InformerBase::nextSequenceNumber() {
    return ++this->currentSequenceNumber; /** TODO(jmoringe): needs atomic increment */
}

}
