/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "ParticipantConfig.h"

#include <stdexcept>

using namespace std;

namespace rsb {

ParticipantConfig::Transport::Transport(const string &name) :
    name(name) {
    if (name.empty()) {
        throw invalid_argument(
                "The name of a transport configuration cannot be empty.");
    }
}

ParticipantConfig::Transport::~Transport() {
}

string ParticipantConfig::Transport::getName() const {
    return name;
}

rsc::runtime::Properties ParticipantConfig::Transport::getOptions() const {
    return options;
}

void ParticipantConfig::Transport::setOptions(
        const rsc::runtime::Properties &options) {
    this->options = options;
}

bool ParticipantConfig::Transport::operator==(const Transport &other) const {
    return name == other.name;
}

bool ParticipantConfig::Transport::operator<(const Transport &other) const {
    return name < other.name;
}

ParticipantConfig::ParticipantConfig() {
}

ParticipantConfig::~ParticipantConfig() {
}

QualityOfServiceSpec ParticipantConfig::getQualityOfServiceSpec() const {
    return qosSpec;
}

void ParticipantConfig::setQualityOfServiceSpec(
        const QualityOfServiceSpec &spec) {
    this->qosSpec = spec;
}

set<ParticipantConfig::Transport> ParticipantConfig::getTransports() const {
    return transports;
}

void ParticipantConfig::addTransport(const Transport &transport) {
    transports.insert(transport);
}

void ParticipantConfig::removeTransport(const Transport &transport) {
    transports.erase(transport);
}

void ParticipantConfig::setTransports(const set<Transport> &transports) {
    this->transports = transports;
}

rsc::runtime::Properties ParticipantConfig::getOptions() const {
    return options;
}

void ParticipantConfig::setOptions(const rsc::runtime::Properties &options) {
    this->options = options;
}

ostream &operator<<(ostream &stream,
        const ParticipantConfig::Transport &transport) {
    return stream << "Transport[name = " << transport.getName() << "]";
}
ostream &operator<<(ostream &stream, const ParticipantConfig &config) {
    stream << "ParticipantConfig[qosSpec = " << config.qosSpec
            << ", transports = [ ";
    for (set<ParticipantConfig::Transport>::const_iterator transportIt =
            config.transports.begin(); transportIt != config.transports.end(); ++transportIt) {
        stream << "'" << transportIt->getName() << "' ";
    }
    stream << " ]]";
    return stream;
}

}
