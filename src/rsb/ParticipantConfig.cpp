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
#include <fstream>

#include <boost/format.hpp>
#include <boost/filesystem/fstream.hpp>

#include <rsc/config/TypedValue.h>
#include <rsc/config/ConfigFileSource.h>
#include <rsc/config/Environment.h>
#include <rsc/logging/Logger.h>

using namespace std;

using namespace boost;
using namespace boost::filesystem;

using namespace rsc::config;
using namespace rsc::logging;
using namespace rsc::runtime;

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

void ParticipantConfig::Transport::setOptions(const Properties &options) {
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

ParticipantConfig::Transport ParticipantConfig::getTransport(const string &name) const {
    map<string, Transport>::const_iterator it = this->transports.find(name);
    if (it == this->transports.end()) {
        throw rsc::runtime::NoSuchObject(name);
    }
    return it->second;
}

set<ParticipantConfig::Transport> ParticipantConfig::getTransports() const {
    set<Transport> result;
    for (map<string, Transport>::const_iterator it
             = this->transports.begin();
         it != this->transports.end(); ++it) {
        result.insert(it->second);
    }
    return result;
}

void ParticipantConfig::addTransport(const Transport &transport) {
    transports.insert(make_pair(transport.getName(), transport));
}

void ParticipantConfig::removeTransport(const Transport &transport) {
    transports.erase(transport.getName());
}

void ParticipantConfig::setTransports(const set<Transport> &transports) {
    for (set<Transport>::const_iterator it = transports.begin();
         it != transports.end(); ++it) {
        this->transports.insert(make_pair(it->getName(), *it));
    }
}

rsc::runtime::Properties ParticipantConfig::getOptions() const {
    return options;
}

void ParticipantConfig::setOptions(const Properties &options) {
    this->options = options;
}

ParticipantConfig ParticipantConfig::fromFile(const path &path,
                                              const ParticipantConfig &defaults) {
    LoggerPtr logger = Logger::getLogger("rsb.ParticipantConfig");
    RSCDEBUG(logger, "Trying to load config from file " << path);

    ParticipantConfig result = defaults;

    boost::filesystem::ifstream stream(path);
    if (stream) {
        RSCDEBUG(logger, "Stream is open; proceeding");
        ConfigFileSource(stream).emit(result);
    } else {
        RSCDEBUG(logger, "Could not open file");
    }

    return result;
}

ParticipantConfig ParticipantConfig::fromEnvironment(const ParticipantConfig &defaults) {
    ParticipantConfig result = defaults;
    EnvironmentVariableSource("RSB_").emit(result);
    return result;
}

ParticipantConfig ParticipantConfig::fromConfiguration(const ParticipantConfig &defaults) {
    ParticipantConfig result = defaults;
    result = fromFile(userConfigDirectory() / "rsb.conf", result);
    result = fromFile("rsb.conf", result);
    result = fromEnvironment(result);
    return result;
}

void ParticipantConfig::handleOption(const vector<string> &key, const string &value) {
    if (key[0] == "qualityofservice") {
        if (key[1] == "reliability") {
            if (value == "UNRELIABLE") {
                this->qosSpec.reliability = QualityOfServiceSpec::UNRELIABLE;
            } else if (value == "RELIABLE") {
                this->qosSpec.reliability = QualityOfServiceSpec::RELIABLE;
            } else
                throw invalid_argument(str(format("The value `%1%' is invalid for the key `qualityofservicespec.reliability'.")
                                           % value));
        } else if (key[1] == "ordering") {
            if (value == "UNORDERED") {
                this->qosSpec.ordering = QualityOfServiceSpec::UNORDERED;
            } else if (value == "ORDERED") {
                this->qosSpec.ordering = QualityOfServiceSpec::ORDERED;
            } else
                throw invalid_argument(str(format("The value `%1%' is invalid for the key `qualityofservicespec.reliability'.")
                                           % value));
        }
    } else if (key[0] == "transport") {
        if (key.size() != 3) {
            throw invalid_argument(str(format("Option key `%1%' has invalid number of components; transport-related keys have to have three components.")
                                       % key));
        }
        map<string, Transport>::iterator it = this->transports.find(key[1]);
        if (it == this->transports.end()) {
            addTransport(Transport(key[1]));
            it = this->transports.find(key[1]);
        }
        Transport& transport = it->second;
        transport.options[key[2]] = parseTypedValue(value);
    } else {
        if (key.size() != 1) {
            throw invalid_argument(str(format("Option key `%1%' seems to designate a global option but has %2% components; global keys must not have more than one component.")
                                       % key % key.size()));
        }
        this->options[key[0]] = parseTypedValue(value);
    }
}

ostream &operator<<(ostream &stream,
        const ParticipantConfig::Transport &transport) {
    return stream << "Transport[name = " << transport.getName() << ", options = " << transport.getOptions() << "]";
}
ostream &operator<<(ostream &stream, const ParticipantConfig &config) {
    stream << "ParticipantConfig[qosSpec = " << config.qosSpec
           << ", transports = " << config.getTransports()
           << ", options = " << config.getOptions()
           << "]";
    return stream;
}

}
