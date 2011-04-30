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

#include <iostream>

#include <string.h>
#include <math.h>

#include <rsc/misc/Registry.h>

#include "../../protocol/Notification.h"
#include "SpreadConnector.h"
#include "SpreadConnection.h"
#include "../../CommException.h"
#include "../../converter/Converter.h"
#include "../../UnsupportedQualityOfServiceException.h"

using namespace std;
using namespace rsc::logging;
using namespace rsc::runtime;
using namespace rsb;
using namespace rsb::transport;
using namespace rsc::threading;

namespace rsb {
namespace spread {

const SpreadConnector::QoSMap SpreadConnector::qosMapping =
    SpreadConnector::buildQoSMapping();

SpreadConnector::SpreadConnector(const std::string &host, unsigned int port,
                                 rsb::converter::Repository<std::string>::Ptr converters) :
    converters(converters) {
    init(host, port);
}

void SpreadConnector::init(const std::string& host, unsigned int port) {
    this->logger = Logger::getLogger("rsb.spread.SpreadConnector");
    RSCDEBUG(logger, "SpreadConnector::init() entered");
    this->activated = false;
    // TODO ConnectionPool for SpreadConnections?!?
    // TODO Send Message over Managing / Introspection Channel
    // TODO Generate Unique-IDs for Connectors
    this->con = SpreadConnectionPtr(new SpreadConnection(id.getIdAsString(), host, port));
    this->memberships = MembershipManagerPtr(new MembershipManager());
    setQualityOfServiceSpecs(QualityOfServiceSpec());
}

void SpreadConnector::activate() {
    // connect to spread
    this->con->activate();
    this->activated = true;
}

void SpreadConnector::deactivate() {
    RSCDEBUG(logger, "deactivate() entered");
    this->con->deactivate();
    // memberships->leaveAll();
    RSCTRACE(logger, "deactivate() finished"); // << *id);
    this->activated = false;
}

void SpreadConnector::join(const std::string &name) {
    this->memberships->join(name, this->con);
}

void SpreadConnector::leave(const std::string &name) {
    this->memberships->leave(name, this->con);
}

bool SpreadConnector::send(const SpreadMessage &msg) {
    return this->con->send(msg);
}

SpreadConnector::~SpreadConnector() {
    if (this->activated) {
        deactivate();
    }
}

SpreadConnectionPtr SpreadConnector::getConnection() {
    return this->con;
}

rsb::converter::Repository<std::string>::Ptr SpreadConnector::getConverters() {
    return this->converters;
}

SpreadMessage::QOS SpreadConnector::getMessageQoS() const {
    return this->messageQoS;
}

SpreadConnector::QoSMap SpreadConnector::buildQoSMapping() {
    map<QualityOfServiceSpec::Reliability, SpreadMessage::QOS> unorderedMap;
    unorderedMap.insert(
        make_pair(QualityOfServiceSpec::UNRELIABLE,
                  SpreadMessage::UNRELIABLE));
    unorderedMap.insert(
        make_pair(QualityOfServiceSpec::RELIABLE, SpreadMessage::RELIABLE));

    map<QualityOfServiceSpec::Reliability, SpreadMessage::QOS> orderedMap;
    orderedMap.insert(
        make_pair(QualityOfServiceSpec::UNRELIABLE, SpreadMessage::FIFO));
    orderedMap.insert(
        make_pair(QualityOfServiceSpec::RELIABLE, SpreadMessage::FIFO));

    map<QualityOfServiceSpec::Ordering, map<QualityOfServiceSpec::Reliability,
        SpreadMessage::QOS> > table;
    table.insert(make_pair(QualityOfServiceSpec::UNORDERED, unorderedMap));
    table.insert(make_pair(QualityOfServiceSpec::ORDERED, orderedMap));

    return table;
}

void SpreadConnector::setQualityOfServiceSpecs(
    const QualityOfServiceSpec &specs) {

    QoSMap::const_iterator orderMapIt = qosMapping.find(specs.getOrdering());
    if (orderMapIt == qosMapping.end()) {
        throw UnsupportedQualityOfServiceException("Unknown ordering", specs);
    }
    map<QualityOfServiceSpec::Reliability, SpreadMessage::QOS>::const_iterator
        mapIt = orderMapIt->second.find(specs.getReliability());
    if (mapIt == orderMapIt->second.end()) {
        throw UnsupportedQualityOfServiceException("Unknown reliability", specs);
    }

    messageQoS = mapIt->second;

    RSCDEBUG(logger, "Selected new message type " << messageQoS);
}

}
}
