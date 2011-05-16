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

ParticipantConfig::Transport::ConverterNames ParticipantConfig::Transport::getConverters() const {
    return this->converters;
}

rsc::runtime::Properties ParticipantConfig::Transport::getOptions() const {
    return options;
}

void ParticipantConfig::Transport::setOptions(const Properties &options) {
    this->options = options;
}

bool ParticipantConfig::Transport::isEnabled() const {
    return this->getOptions().getAs<bool> ("enabled", true);
}

void ParticipantConfig::Transport::handleOption(const vector<string> &key,
        const string &value) {
    if (key[0] == "converter") {
        if (key.size() != 2) {
            throw invalid_argument(
                    str(
                            format(
                                    "Option key `%1%' has invalid number of components; converter-related keys for transports has to have two components")
                                    % key));
        }
        this->converters.insert(make_pair(key[1], value));
    } else {
        if (key.size() != 1) {
            throw invalid_argument(
                    str(
                            format(
                                    "Key `%1%' has invalid number of components; transport option keys have to have one component.")
                                    % key));
        }
        this->options[key[0]] = value;
    }
}

bool ParticipantConfig::Transport::operator==(const Transport &other) const {
    return name == other.name;
}

bool ParticipantConfig::Transport::operator<(const Transport &other) const {
    return name < other.name;
}

string ParticipantConfig::Transport::getClassName() const {
    return "Transport";
}

void ParticipantConfig::Transport::printContents(ostream &stream) const {
    stream << "name = " << this->name << ", converters = " << this->converters
            << ", options = " << this->options;
}

ParticipantConfig::ParticipantConfig() :
    logger(Logger::getLogger("rsb.ParticipantConfig")), errorStrategy(LOG) {
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

ParticipantConfig::ErrorStrategy ParticipantConfig::getErrorStrategy() const {
    return errorStrategy;
}

void ParticipantConfig::setErrorStrategy(const ErrorStrategy &strategy) {
    this->errorStrategy = strategy;
}

ParticipantConfig::Transport ParticipantConfig::getTransport(const string &name) const {
    map<string, Transport>::const_iterator it = this->transports.find(name);
    if (it == this->transports.end()) {
        throw rsc::runtime::NoSuchObject(name);
    }
    return it->second;
}

set<ParticipantConfig::Transport> ParticipantConfig::getTransports(
        bool includeDisabled) const {
    set<Transport> result;
    for (map<string, Transport>::const_iterator it = this->transports.begin(); it
            != this->transports.end(); ++it) {
        if (it->second.isEnabled() || includeDisabled) {
            result.insert(it->second);
        } else {
            RSCDEBUG(logger, "Skipping disabled transport " << it->second);
        }
    }
    return result;
}

void ParticipantConfig::addTransport(const Transport &transport) {
    transports.erase(transport.getName());
    transports.insert(make_pair(transport.getName(), transport));
}

void ParticipantConfig::removeTransport(const Transport &transport) {
    transports.erase(transport.getName());
}

void ParticipantConfig::setTransports(const set<Transport> &transports) {
    for (set<Transport>::const_iterator it = transports.begin(); it
            != transports.end(); ++it) {
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

ParticipantConfig ParticipantConfig::fromEnvironment(
        const ParticipantConfig &defaults) {
    ParticipantConfig result = defaults;
    EnvironmentVariableSource("RSB_").emit(result);
    return result;
}

ParticipantConfig ParticipantConfig::fromConfiguration(
        const ParticipantConfig &defaults) {
    ParticipantConfig result = defaults;
    try {
        result = fromFile(userConfigDirectory() / "rsb.conf", result);
    } catch (runtime_error &e) {
        RSCWARN(Logger::getLogger("rsb.ParticipantConfig"), "Could not find a user config directory for parsing the config file.");
    }
    result = fromFile("rsb.conf", result);
    result = fromEnvironment(result);
    return result;
}

void ParticipantConfig::handleOption(const vector<string> &key,
        const string &value) {
    // Quality of Service
    if (key[0] == "qualityofservice") {
        if (key.size() != 2) {
            throw invalid_argument(
                    str(
                            format(
                                    "Option key `%1%' has invalid number of components; options related to quality of service have to have two components.")
                                    % key));
        }
        if (key[1] == "reliability") {
            if (value == "UNRELIABLE") {
                this->qosSpec.reliability = QualityOfServiceSpec::UNRELIABLE;
            } else if (value == "RELIABLE") {
                this->qosSpec.reliability = QualityOfServiceSpec::RELIABLE;
            } else
                throw invalid_argument(str(format(
                        "The value `%1%' is invalid for the key `%2%'.")
                        % value % key));
        } else if (key[1] == "ordering") {
            if (value == "UNORDERED") {
                this->qosSpec.ordering = QualityOfServiceSpec::UNORDERED;
            } else if (value == "ORDERED") {
                this->qosSpec.ordering = QualityOfServiceSpec::ORDERED;
            } else
                throw invalid_argument(str(format(
                        "The value `%1%' is invalid for the key `%2%'.")
                        % value % key));
        } else {
            throw invalid_argument(
                    str(format("`%2%' is not a valid sub-key of `%1%'.")
                            % key[0] % key[1]));
        }
        // Error handling
    } else if (key[0] == "errorhandling") {
        if (key[1] == "onhandlererror") {
            if (value == "LOG") {
                this->errorStrategy = LOG;
            } else if (value == "PRINT") {
                this->errorStrategy = PRINT;
            } else if (value == "EXIT") {
                this->errorStrategy = EXIT;
            } else {
                throw invalid_argument(str(format(
                        "The value `%1%' is invalid for the key `%2%'.")
                        % value % key));
            }
        } else {
            throw invalid_argument(
                    str(format("`%2%' is not a valid sub-key of `%1%'.")
                            % key[0] % key[1]));
        }
        // Transports
    } else if (key[0] == "transport") {
        if (key.size() < 3) {
            throw invalid_argument(
                    str(
                            format(
                                    "Option key `%1%' has invalid number of components; transport-related keys have to have at least three components.")
                                    % key));
        }
        map<string, Transport>::iterator it = this->transports.find(key[1]);
        if (it == this->transports.end()) {
            addTransport(Transport(key[1]));
            it = this->transports.find(key[1]);
        }
        Transport& transport = it->second;
        vector<string> subKey;
        copy(key.begin() + 2, key.end(), back_inserter(subKey));
        transport.handleOption(subKey, value);
        // Global (participant-wide) options
    } else {
        if (key.size() != 1) {
            throw invalid_argument(
                    str(
                            format(
                                    "Option key `%1%' seems to designate a global option but has %2% components; global keys must not have more than one component.")
                                    % key % key.size()));
        }
        this->options[key[0]] = parseTypedValue(value);
    }
}

ostream &operator<<(ostream &stream, const ParticipantConfig &config) {
    stream << "ParticipantConfig[qosSpec = " << config.qosSpec
            << ", errorStrategy = " << config.errorStrategy
            << ", transports = " << config.getTransports() << ", options = "
            << config.getOptions() << "]";
    return stream;
}

}
