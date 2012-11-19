/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2012 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "Factory.h"

#include <boost/filesystem/fstream.hpp>

#include <rsc/config/Configuration.h>
#include <rsc/config/Environment.h>

#include <rsc/logging/OptionBasedConfigurator.h>

#include <rsc/plugins/Manager.h>
#include <rsc/plugins/Configurator.h>

#include <rsb/Version.h>

#include "eventprocessing/strategies.h"

#include "converter/converters.h"
#include "converter/Repository.h"
#include "converter/UnambiguousConverterMap.h"

#include "transport/transports.h"

#include "LocalService.h"

using namespace std;

using namespace rsc::config;
using namespace rsc::logging;
using namespace rsc::runtime;

using namespace rsb::converter;
using namespace rsb::transport;

namespace {

template<unsigned int which, typename C>
std::map<typename C::value_type::first_type,
         typename C::value_type::second_type> pairsToMap(const C& container) {
    typedef typename C::value_type::first_type first_type;
    typedef typename C::value_type::second_type second_type;

    typedef typename C::const_iterator const_iterator;

    std::map<first_type, second_type> result;
    for (const_iterator it = container.begin(); it != container.end(); ++it) {
        if (which == 1)
            result[it->first] = it->second;
        else
            result[it->second] = it->first;
        }
    return result;
}

}

namespace rsb {

Factory& Factory::getInstance() {
    return getFactory();
}

Factory& getFactory() {
    return Factory::getInstanceBase();
}

Factory& Factory::getInstanceBase() {
    return rsc::patterns::Singleton<Factory>::getInstance();
}

Factory::Factory() :
    logger(Logger::getLogger("rsb.Factory")) {

    // Configure RSC-based logging.
    {
        rsc::logging::OptionBasedConfigurator configurator;
        configure(configurator, "rsb.conf", "RSC_");
    }

    // Register default implementation for all extension points.
    RSCINFO(this->logger, "Registering default implementations");
    eventprocessing::registerDefaultEventProcessingStrategies();
    converter::registerDefaultConverters();
    transport::registerDefaultTransports();

    // Configure plugin path and load plugins to register additional
    // implementations for extension points.
    //
    // We use the following default plugin path:
    // 1. $HOME/.rsb$MAJOR.$MINOR/plugins
    // 2. $libdir/rsb$MAJOR.$MINOR/plugins
    RSCINFO(this->logger, "Processing plugin configuration");
    {
        string versioned = str(boost::format("rsb%1%.%2%")
                               % RSB_VERSION_MAJOR
                               % RSB_VERSION_MINOR);
        vector<boost::filesystem::path> defaultPath;
        // It may be impossible to determine a home directory for the
        // current user. Warn, but don't throw.
        try {
            defaultPath.push_back(userHomeDirectory() / ("." + versioned) / "plugins");
        } catch (const runtime_error& e) {
            RSCWARN(this->logger,
                    "Failed to determine user-specific plugin directory: "
                    << e.what());
        }
        defaultPath.push_back(Version::libdir() / versioned / "plugins");
        rsc::plugins::Configurator configurator(defaultPath);
        configure(configurator, "rsb.conf", "RSB_");
    }

    // Setup default participant config
    //
    // Collect all available connector implementations from the
    // connector factories:
    // + In-push
    // + In-pull
    // + Out
    // Disable discovered connectors with the exception of the
    // socket transport.
    set<string> availableTransports;
    {
        set<InPullFactory::ConnectorInfo> infos = getInPullFactory().getConnectorInfos();
        for (set<InPullFactory::ConnectorInfo>::const_iterator it
                 = infos.begin(); it != infos.end(); ++it) {
            availableTransports.insert(it->getName());
        }
    }{
        set<InPushFactory::ConnectorInfo> infos = getInPushFactory().getConnectorInfos();
        for (set<InPushFactory::ConnectorInfo>::const_iterator it
                 = infos.begin(); it != infos.end(); ++it) {
            availableTransports.insert(it->getName());
        }
    }{
        set<OutFactory::ConnectorInfo> infos = getOutFactory().getConnectorInfos();
        for (set<OutFactory::ConnectorInfo>::const_iterator it
                 = infos.begin(); it != infos.end(); ++it) {
            availableTransports.insert(it->getName());
        }
    }

    this->defaultConfig = ParticipantConfig();
    for (set<string>::const_iterator it = availableTransports.begin();
         it != availableTransports.end(); ++it) {
        this->defaultConfig.addTransport(ParticipantConfig::Transport(*it, *it == "socket"));
    }

    // If there is only one transport, we can blindly enable it since
    // the user could end up without any enabled transport otherwise.
    if (this->defaultConfig.getTransports().size() == 1) {
        string name = this->defaultConfig.getTransports().begin()->getName();
        this->defaultConfig.mutableTransport(name).setEnabled(true);
    }

    // Merge with user configuration (configuration files, environment
    // variables)
    configure(this->defaultConfig, "rsb.conf", "RSB_");

    // Issue a warning if the combination of available transport
    // implementations and user configuration leads to no enabled
    // transports.
    if (this->defaultConfig.getTransports().empty()) {
        RSCWARN(logger, "No transports are enabled. This is probably a configuration error or an internal RSB error.");
    }

    RSCDEBUG(logger, "Default config " << defaultConfig);
}

Factory::~Factory() {
}

InformerBasePtr Factory::createInformerBase(const Scope&             scope,
                                            const string&            dataType,
                                            const ParticipantConfig& config) {
    return InformerBasePtr(new InformerBase(createOutConnectors(config), scope, config, dataType));
}


ListenerPtr Factory::createListener(const Scope& scope,
        const ParticipantConfig& config) {
    return ListenerPtr(new Listener(createInPushConnectors(config), scope, config));
}

ReaderPtr Factory::createReader(const Scope& scope,
                                const ParticipantConfig& config) {
    return ReaderPtr(new Reader(createInPullConnectors(config), scope, config));
}

patterns::ServerPtr Factory::createServer(const Scope& scope,
        const ParticipantConfig &listenerConfig,
        const ParticipantConfig &informerConfig) {
    return patterns::ServerPtr(
            new patterns::Server(scope, listenerConfig, informerConfig));
}

patterns::RemoteServerPtr Factory::createRemoteServer(const Scope& scope,
        const ParticipantConfig &listenerConfig,
        const ParticipantConfig &informerConfig) {
    return patterns::RemoteServerPtr(
            new patterns::RemoteServer(scope, listenerConfig, informerConfig));
}

ParticipantConfig Factory::getDefaultParticipantConfig() const {
    boost::recursive_mutex::scoped_lock lock(configMutex);
    return defaultConfig;
}

void Factory::setDefaultParticipantConfig(const ParticipantConfig& config) {
    boost::recursive_mutex::scoped_lock lock(configMutex);
    this->defaultConfig = config;
}

ServicePtr Factory::createService(const Scope& scope) {
    return ServicePtr(new LocalService(scope));
}

vector<InPullConnectorPtr>
Factory::createInPullConnectors(const ParticipantConfig& config) {
    // Note: getTransports() only returns *enabled* transports.
    vector<InPullConnectorPtr> connectors;
    set<ParticipantConfig::Transport> configuredTransports = config.getTransports();
    for (set<ParticipantConfig::Transport>::const_iterator transportIt =
             configuredTransports.begin(); transportIt
             != configuredTransports.end(); ++transportIt) {
        RSCDEBUG(logger, "Trying to add connector " << *transportIt);
        Properties options = transportIt->getOptions();
        RSCDEBUG(logger, "Supplied connector options " << transportIt->getOptions());

        // Take care of converters
        if (!options.has("converters")) {
            RSCDEBUG(logger, "Converter configuration for transport `"
                     << transportIt->getName() << "': " << transportIt->getConverters());
            // TODO we should not have to know the transport's wire-type here
            ConverterSelectionStrategy<string>::Ptr converters
                = converterRepository<string>()
                ->getConvertersForDeserialization(pairsToMap<1> (transportIt->getConverters()));
            RSCDEBUG(logger, "Selected converters for transport `"
                     << transportIt->getName() << "': " << converters);
            options["converters"] = converters;
        }
        connectors.push_back(InPullConnectorPtr(getInPullFactory().createInst(transportIt->getName(), options)));
    }
    return connectors;
}

vector<InPushConnectorPtr>
Factory::createInPushConnectors(const ParticipantConfig& config) {
    // Note: getTransports() only returns *enabled* transports.
    vector<InPushConnectorPtr> connectors;
    set<ParticipantConfig::Transport> configuredTransports = config.getTransports();
    for (set<ParticipantConfig::Transport>::const_iterator transportIt =
             configuredTransports.begin(); transportIt
             != configuredTransports.end(); ++transportIt) {
        RSCDEBUG(logger, "Trying to add connector " << *transportIt);
        Properties options = transportIt->getOptions();
        RSCDEBUG(logger, "Supplied connector options " << transportIt->getOptions());

        // Take care of converters
        if (!options.has("converters")) {
            RSCDEBUG(logger, "Converter configuration for transport `"
                     << transportIt->getName() << "': " << transportIt->getConverters());
            // TODO we should not have to know the transport's wire-type here
            ConverterSelectionStrategy<string>::Ptr converters
                = converterRepository<string>()
                ->getConvertersForDeserialization(pairsToMap<1> (transportIt->getConverters()));
            RSCDEBUG(logger, "Selected converters for transport `"
                     << transportIt->getName() << "': " << converters);
            options["converters"] = converters;
        }
        connectors.push_back(InPushConnectorPtr(getInPushFactory().createInst(transportIt->getName(), options)));
    }
    return connectors;
}

vector<OutConnectorPtr>
Factory::createOutConnectors(const ParticipantConfig& config) {
    // Note: getTransports() only returns *enabled* transports.
    vector<OutConnectorPtr> connectors;
    set<ParticipantConfig::Transport> configuredTransports = config.getTransports();
    for (set<ParticipantConfig::Transport>::const_iterator transportIt =
             configuredTransports.begin(); transportIt
             != configuredTransports.end(); ++transportIt) {
        RSCDEBUG(logger, "Trying to add connector " << *transportIt);
        Properties options = transportIt->getOptions();
        RSCDEBUG(logger, "Supplied connector options " << transportIt->getOptions());

        // Take care of converters
        if (!options.has("converters")) {
            RSCDEBUG(logger, "Converter configuration for transport `"
                     << transportIt->getName() << "': " << transportIt->getConverters());
            // TODO we should not have to know the transport's wire-type here
            ConverterSelectionStrategy<string>::Ptr converters
                = converterRepository<string>()
                ->getConvertersForSerialization(pairsToMap<2> (transportIt->getConverters()));
            RSCDEBUG(logger, "Selected converters for transport `"
                     << transportIt->getName() << "': " << converters);
            options["converters"] = converters;
        }
        connectors.push_back(OutConnectorPtr(getOutFactory().createInst(transportIt->getName(), options)));
    }
    return connectors;
}

}
