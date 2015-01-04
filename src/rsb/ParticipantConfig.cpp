/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "ParticipantConfig.h"

#include <stdexcept>
#include <fstream>

#include <boost/format.hpp>
#include <boost/filesystem/fstream.hpp>

#include <rsc/logging/Logger.h>

using namespace std;

using namespace boost;
using namespace boost::filesystem;

using namespace rsc::config;
using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {

ParticipantConfig::Transport::Transport(const string& name,
                                        bool enabled) :
    name(name) {
    if (name.empty()) {
        throw invalid_argument(
                "The name of a transport configuration cannot be empty.");
    }
    if (!enabled) {
        setEnabled(false);
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

void ParticipantConfig::Transport::addConverter(const string &wireschema, const string &datatype)
{
    this->converters.insert(make_pair(wireschema, datatype));
}

void ParticipantConfig::Transport::addConverters(const ParticipantConfig::Transport::ConverterNames &converters)
{
    this->converters.insert(converters.begin(), converters.end());
}

rsc::runtime::Properties ParticipantConfig::Transport::getOptions() const {
    return this->options;
}

rsc::runtime::Properties& ParticipantConfig::Transport::mutableOptions() {
    return this->options;
}


void ParticipantConfig::Transport::setOptions(const Properties& options) {
    this->options = options;
}

bool ParticipantConfig::Transport::isEnabled() const {
    return this->getOptions().getAs<bool> ("enabled", true);
}

void ParticipantConfig::Transport::setEnabled(bool value) {
    Properties options = this->getOptions();
    options["enabled"] = lexical_cast<string>(value);
    setOptions(options);
}

void ParticipantConfig::Transport::handleOption(const vector<string>& key,
        const string& value) {
    if ((key.size() >= 2)
        && (key[0] == "converter")
        && (key[1] == "cpp")) {
        if (key.size() != 3) {
            throw invalid_argument(
                    str(
                            format(
                                    "Option key `%1%' has invalid number of components; converter-related keys for transports has to have three components")
                                    % key));
        }
        this->addConverter(key[2], value);
    } else if ((key.size() >= 2) && (key[0] == "converter")) {
        // ignore converters for other languages
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

void ParticipantConfig::Transport::mergeOptions(const rsc::misc::uri& uri) {
    assert(uri.scheme().empty() || uri.scheme() == this->getName());
    for (rsc::runtime::Properties::const_iterator 
             it=uri.query.begin(), end=uri.query.end(); it!=end; ++it) {
        this->options[it->first] = it->second;
    }
}

bool ParticipantConfig::Transport::operator==(const Transport& other) const {
    return name == other.name;
}

bool ParticipantConfig::Transport::operator<(const Transport& other) const {
    return name < other.name;
}

string ParticipantConfig::Transport::getClassName() const {
    return "Transport";
}

void ParticipantConfig::Transport::printContents(ostream& stream) const {
    stream << "name = " << this->name
           << ", converters = " << this->converters
           << ", options = " << this->options;
}

ParticipantConfig::EventProcessingStrategy::EventProcessingStrategy(const string& name) :
    name(name) {
}

string ParticipantConfig::EventProcessingStrategy::getName() const {
    return this->name;
}

void ParticipantConfig::EventProcessingStrategy::setName(const string& name) {
    this->name = name;
}

Properties ParticipantConfig::EventProcessingStrategy::getOptions() const {
    return this->options;
}

rsc::runtime::Properties& ParticipantConfig::EventProcessingStrategy::mutableOptions() {
    return this->options;
}

void ParticipantConfig::EventProcessingStrategy::setOptions(const Properties& options) {
    this->options = options;
}

void ParticipantConfig::EventProcessingStrategy::handleOption(const vector<string>& key,
                                                              const string& value) {
    if (key.size() != 1) {
        throw invalid_argument(str(format("Key `%1%' has invalid number of components; transport option keys have to have one component.")
                                   % key));
    }

    this->options.set<string>(key.front(), value);
}

void ParticipantConfig::EventProcessingStrategy::printContents(ostream& stream) const {
    stream << "name = " << this->name
           << ", options = " << this->options;
}

ParticipantConfig::ParticipantConfig() :
    logger(Logger::getLogger("rsb.ParticipantConfig")),
    errorStrategy(ERROR_STRATEGY_LOG),
    eventReceivingStrategy("parallel"),
    eventSendingStrategy("direct"),
    introspection(false) {
}

ParticipantConfig::~ParticipantConfig() {
}

QualityOfServiceSpec ParticipantConfig::getQualityOfServiceSpec() const {
    return qosSpec;
}

QualityOfServiceSpec& ParticipantConfig::mutableQualityOfServiceSpec() {
    return qosSpec;
}

void ParticipantConfig::setQualityOfServiceSpec(
        const QualityOfServiceSpec& spec) {
    this->qosSpec = spec;
}

ParticipantConfig::ErrorStrategy ParticipantConfig::getErrorStrategy() const {
    return errorStrategy;
}

void ParticipantConfig::setErrorStrategy(const ErrorStrategy& strategy) {
    this->errorStrategy = strategy;
}

ParticipantConfig::Transport ParticipantConfig::getTransport(const string& name) const {
    map<string, Transport>::const_iterator it = this->transports.find(name);
    if (it == this->transports.end()) {
        throw rsc::runtime::NoSuchObject(name);
    }
    return it->second;
}

ParticipantConfig::Transport& ParticipantConfig::mutableTransport(const string& name) {
    map<string, Transport>::iterator it = this->transports.find(name);
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

void ParticipantConfig::addTransport(const Transport& transport) {
    transports.erase(transport.getName());
    transports.insert(make_pair(transport.getName(), transport));
}

void ParticipantConfig::removeTransport(const Transport& transport) {
    transports.erase(transport.getName());
}

void ParticipantConfig::setTransports(const set<Transport>& transports) {
    for (set<Transport>::const_iterator it = transports.begin(); it
            != transports.end(); ++it) {
        this->transports.insert(make_pair(it->getName(), *it));
    }
}

void ParticipantConfig::addTransport (const rsc::misc::uri& uri,
                                      bool disableOthers) {
    if (!uri.scheme().empty()) {
        // a concrete transport was specified
        if (disableOthers) {
            for (map<string, Transport>::iterator
                     it = this->transports.begin(), end = this->transports.end();
                 it != end; ++it) {
                it->second.setEnabled(false);
            }
        }

        Transport& t = (*(this->transports.insert(
                              make_pair(uri.scheme(), Transport(uri.scheme(), true))).first)).second;
        t.mergeOptions (uri); // merge in uri parameters
    } else {
        if (uri.query.has("host") || uri.query.has("port"))
            throw std::invalid_argument("invalid transport spec: "
                                        "host or port options given, but no transport specified");

        // uri scheme = transport is empty: configure all existing transports
        for (std::map<std::string, Transport>::iterator
                 it=this->transports.begin(), end=this->transports.end(); it!=end; ++it) {
            it->second.mergeOptions(uri);
        }
    }
}

void ParticipantConfig::setTransports (const std::set<rsc::misc::uri>& uris) {
    // check for empty uri schemes = undefined transports first
    for (std::set<rsc::misc::uri>::const_iterator it=uris.begin(), end=uris.end();
         it != end; ++it) {
        if (it->scheme().empty()) throw std::invalid_argument("undefined transport scheme in set");
    }

    std::map<std::string, Transport> transports;
    for (std::set<rsc::misc::uri>::const_iterator it=uris.begin(), end=uris.end();
         it != end; ++it) {
        // fetch existing or create new, empty transport
        std::map<std::string, Transport>::iterator entry =
            this->transports.insert(
                make_pair(it->scheme(), Transport(it->scheme(), true))).first;
        Transport& t = entry->second;
        t.mergeOptions (*it); // merge in uri parameters
        t.setEnabled(true);
        transports.insert (*entry);
    }
    this->transports.swap(transports); // replace set
}

const ParticipantConfig::EventProcessingStrategy& ParticipantConfig::getEventReceivingStrategy() const {
    return this->eventReceivingStrategy;
}

ParticipantConfig::EventProcessingStrategy& ParticipantConfig::mutableEventReceivingStrategy() {
    return this->eventReceivingStrategy;
}

const ParticipantConfig::EventProcessingStrategy& ParticipantConfig::getEventSendingStrategy() const {
    return this->eventSendingStrategy;
}

bool ParticipantConfig::isIntrospectionEnabled() const {
    return this->introspection;
}

void ParticipantConfig::setIsIntrospectionEnabled(bool newValue) {
    this->introspection = newValue;
}


rsc::runtime::Properties ParticipantConfig::getOptions() const {
    return options;
}

rsc::runtime::Properties& ParticipantConfig::mutableOptions() {
    return options;
}

void ParticipantConfig::setOptions(const Properties& options) {
    this->options = options;
}

void ParticipantConfig::addPreferredConverters(const ParticipantConfig::Transport::ConverterNames &converters)
{
    for (map<string, Transport>::iterator
         it = this->transports.begin(), end = this->transports.end();
         it != end; ++it) {
        it->second.addConverters(converters);
    }
}

void ParticipantConfig::handleOption(const vector<string>& key,
        const string& value) {
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
                this->errorStrategy = ERROR_STRATEGY_LOG;
            } else if (value == "PRINT") {
                this->errorStrategy = ERROR_STRATEGY_PRINT;
            } else if (value == "EXIT") {
                this->errorStrategy = ERROR_STRATEGY_EXIT;
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
        // Event processing
    } else if (key[0] == "eventprocessing") {
        EventProcessingStrategy* strategy;
        if (key[1] == "receivingstrategy") {
            strategy = &this->eventReceivingStrategy;
        } else if (key[1] == "sendingstrategy") {
            strategy = &this->eventSendingStrategy;
        } else {
            throw invalid_argument(
                str(format("`%2%' is not a valid sub-key of `%1%'.")
                    % key[0] % key[1]));
        }
        if (key.size() == 2) {
            strategy->setName(value);
        } else {
            vector<string> subKey;
            copy(key.begin() + 2, key.end(), back_inserter(subKey));
            strategy->handleOption(subKey, value);
        }

    // Introspection
    } else if (key[0] == "introspection") {
        if ((key.size() == 2) && (key[1] == "enabled")) {
            this->introspection = lexical_cast<bool>(value);
        } else {
            throw invalid_argument(str(format("`%2%' is not a valid sub-key of `%1%'.")
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
        if (key.size() == 1) {
            this->options[key[0]] = value;
        }
    }
}

void ParticipantConfig::printContents(std::ostream& stream) const {
    stream << "qosSpec = " << this->qosSpec
           << ", errorStrategy = " << this->errorStrategy
           << ", transports = " << this->getTransports(true)
           << ", eventReceivingStrategy = " << this->eventReceivingStrategy
           << ", eventSendingStrategy = " << this->eventSendingStrategy
           << ", options = " << this->options << "]";
}

}
