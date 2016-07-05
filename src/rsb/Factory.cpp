/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2012, 2013, 2014, 2016 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

using namespace std;

using namespace rsc::config;
using namespace rsc::logging;
using namespace rsc::runtime;

using namespace rsb::converter;
using namespace rsb::transport;

namespace { // anonymous namespace for file-local helper functions.

enum ConverterDirection {
    SERIALIZATION,
    DESERIALIZATION
};

template<typename C>
std::map<typename C::value_type::first_type,
         typename C::value_type::second_type>
converterSelectionToMap(const C& container, ConverterDirection direction) {
    typedef typename C::value_type::first_type first_type;
    typedef typename C::value_type::second_type second_type;

    typedef typename C::const_iterator const_iterator;

    std::map<first_type, second_type> result;
    for (const_iterator it = container.begin(); it != container.end(); ++it) {
        switch (direction) {
        case DESERIALIZATION:
            result[it->first] = it->second;
            break;
        case SERIALIZATION:
            if (result.find(it->second) != result.end()) {
                throw std::invalid_argument(
                        boost::str(
                        boost::format(
                        "Multiple wire-schemas (%1%, %2%) selected for data-type %3%.\n"
                        "Probably you wrote the lines transport.<name>.cpp.%1% = %3% "
                        "and transport.<name>.cpp.%2% = %3% in your rsb config. One of "
                        "these should be deleted.")
                                % it->first
                                % result[it->second]
                                % it->second
                        )
                );
            }
            result[it->second] = it->first;
            break;
        default:
            assert(false);
            throw std::runtime_error("got unexpected serialization direction");
        }
    }
    return result;
}

Properties
prepareConnectorOptions(const rsb::ParticipantConfig::Transport& config,
                        ConverterDirection direction,
                        rsc::logging::LoggerPtr logger) {
    Properties options = config.getOptions();
    RSCDEBUG(logger, "Supplied connector options " << options);

    // For local transport, we do not mess with the converter
    // configuration since it is not used (or even touched) anyway.
    //
    // For remote transports, we build a converter selection strategy
    // - suitable for direction - and put it into the "converters"
    // property of the transport configuration.
    //
    // Note that config.getConverters() only returns /converter
    // disambiguations/.  These are used to guide the actual converter
    // selection in converterSelectionToMap().
    if (rsb::transport::isRemote(config.getName()) && !options.has("converters")) {
        RSCDEBUG(logger, "Converter configuration for transport `"
                 << config.getName() << "': " << config.getConverters());

        ConverterSelectionStrategy<string>::Ptr converters; // TODO we should not have to know the transport's wire-type here
        switch (direction) {
        case SERIALIZATION:
            converters
                = converterRepository<string>() // TODO wire-type
                ->getConvertersForSerialization
                (converterSelectionToMap(config.getConverters(), direction));
            break;
        case DESERIALIZATION:
            converters
                = converterRepository<string>() // TODO wire-type
                ->getConvertersForDeserialization
                (converterSelectionToMap(config.getConverters(), direction));
            break;
        default:
            assert(false);
            throw std::runtime_error("got unexpected serialization direction");
        }
        RSCDEBUG(logger, "Selected converters for transport `"
                 << config.getName() << "': " << converters);
        options["converters"] = converters;
    }

    return options;
}

}

namespace rsb {

Factory* factoryWhileLoadingPlugins = NULL;

Factory& getFactory() {
    if (factoryWhileLoadingPlugins) {
        return *factoryWhileLoadingPlugins;
    } else {
        static Factory factory;
        return factory;
    }
}

struct ConfigDebugPrinter {
    ConfigDebugPrinter(const std::string& phase, bool enabled)
        : phase(phase), enabled(enabled) {
        if (this->enabled){
            std::cerr << "Starting processing " << this->phase << " configuration"
                      << std::endl << std::endl;
        }
    }

    ~ConfigDebugPrinter() {
        if (this->enabled) {
            std::cerr << std::endl
                      << "Finished processing " << this->phase << " configuration"
                      << std::endl << std::endl;
        }
    }

    std::string phase;
    bool        enabled;
};

Factory::Factory() :
    logger(Logger::getLogger("rsb.Factory")),
    pluginManager(new rsc::plugins::Manager()),
    signalParticipantCreated(new SignalParticipantCreated),
    signalParticipantDestroyed(new SignalParticipantDestroyed) {

    bool debugConfig = getEnvironmentVariable("__CONFIG_DEBUG").get();

    // Configure RSC-based logging.
    {
        ConfigDebugPrinter printer("RSC-based logging", debugConfig);

        rsc::logging::OptionBasedConfigurator configurator;
        configure(configurator, "rsb.conf", "RSC_", 0, 0, false, Version::installPrefix());
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
    // 1. $HOME/.$RSB_PLUGIN_PATH_SUFFIX
    // 2. $libdir/$RSB_PLUGIN_PATH_SUFFIX
    RSCINFO(this->logger, "Processing plugin configuration");
    try {
        ConfigDebugPrinter printer("plugin", debugConfig);

        factoryWhileLoadingPlugins = this;

        vector<boost::filesystem::path> defaultPath;
        // It may be impossible to determine a home directory for the
        // current user. Warn, but don't throw.
        try {
            defaultPath.push_back(userHomeDirectory()
                        / ("." + Version::buildPluginPathSuffix()));
        } catch (const runtime_error& e) {
            RSCWARN(this->logger,
                    "Failed to determine user-specific plugin directory: "
                    << e.what());
        }
        defaultPath.push_back(Version::libdir() / Version::buildPluginPathSuffix());
        rsc::plugins::Configurator configurator(pluginManager, defaultPath);
        provideConfigOptions(configurator);
        configurator.execute(true);
    } catch (...) {
        factoryWhileLoadingPlugins = NULL;
        throw;
    }
    factoryWhileLoadingPlugins = NULL;

    // Setup default participant config
    //
    // Collect all available connector implementations:
    // + In-push
    // + In-pull
    // + Out
    // Disable discovered connectors with the exception of the
    // socket transport.
    set<string> availableTransports = getAvailableTransports(DIRECTION_IN_PUSH
                                                             | DIRECTION_IN_PULL
                                                             | DIRECTION_OUT);

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
    {
        ConfigDebugPrinter printer("default participant", debugConfig);

        provideConfigOptions(this->defaultConfig);
    }
    if (debugConfig) {
        std::cerr << "Default participant configuration" << std::endl
                  << defaultConfig << std::endl << std::endl;
    }

    // Issue a warning if the combination of available transport
    // implementations and user configuration leads to no enabled
    // transports.
    if (this->defaultConfig.getTransports().empty()) {
        RSCWARN(logger, "No transports are enabled. This is probably a"
                        " configuration error or an internal RSB error.");
    }

    RSCDEBUG(logger, "Default config " << defaultConfig);
}

Factory::~Factory() {
}

void Factory::provideConfigOptions(OptionHandler& handler) {
    configure(handler, "rsb.conf", "RSB_", 0, 0, true,
            Version::installPrefix());
}

SignalParticipantCreatedPtr Factory::getSignalParticipantCreated() {
    return this->signalParticipantCreated;
}

SignalParticipantDestroyedPtr Factory::getSignalParticipantDestroyed() {
    return this->signalParticipantDestroyed;
}

InformerBasePtr Factory::createInformerBase(const Scope&             scope,
                                            const string&            dataType,
                                            const ParticipantConfig& config,
                                            ParticipantPtr           parent) {
    InformerBasePtr informer(
        new InformerBase(createOutConnectors(config), scope, config, dataType));
    informer->setSignalParticipantDestroyed(this->signalParticipantDestroyed);
    (*this->signalParticipantCreated)(informer, parent);
    return informer;
}


ListenerPtr Factory::createListener(const Scope&             scope,
                                    const ParticipantConfig& config,
                                    ParticipantPtr           parent) {
    ListenerPtr listener(
        new Listener(createInPushConnectors(config), scope, config));
    listener->setSignalParticipantDestroyed(this->signalParticipantDestroyed);
    (*this->signalParticipantCreated)(listener, parent);
    return listener;
}

ReaderPtr Factory::createReader(const Scope&             scope,
                                const ParticipantConfig& config,
                                ParticipantPtr           parent) {
    ReaderPtr reader(
        new Reader(createInPullConnectors(config), scope, config));
    reader->setSignalParticipantDestroyed(this->signalParticipantDestroyed);
    (*this->signalParticipantCreated)(reader, parent);
    return reader;
}

patterns::LocalServer::LocalMethodPtr Factory::createLocalMethod
(const Scope&                       scope,
 patterns::LocalServer::CallbackPtr callback,
 const ParticipantConfig&           listenerConfig,
 const ParticipantConfig&           informerConfig,
 ParticipantPtr                     parent) {
    patterns::LocalServer::LocalMethodPtr method
        (new patterns::LocalServer::LocalMethod
         (scope, scope.getComponents()[scope.getComponents().size() -1],
          listenerConfig, informerConfig, callback));
    method->setSignalParticipantDestroyed(this->signalParticipantDestroyed);
    (*this->signalParticipantCreated)(method, parent);
    return method;
}

patterns::LocalServerPtr Factory::createLocalServer(const Scope& scope,
                                                    const ParticipantConfig &listenerConfig,
                                                    const ParticipantConfig &informerConfig,
                                                    ParticipantPtr          parent) {
    patterns::LocalServerPtr server(
        new patterns::LocalServer(scope, listenerConfig, informerConfig));
    server->setSignalParticipantDestroyed(this->signalParticipantDestroyed);
    (*this->signalParticipantCreated)(server, parent);
    return server;
}

patterns::RemoteServer::RemoteMethodPtr Factory::createRemoteMethod
(const Scope&             scope,
 const ParticipantConfig& listenerConfig,
 const ParticipantConfig& informerConfig,
 ParticipantPtr           parent) {
    patterns::RemoteServer::RemoteMethodPtr method
        (new patterns::RemoteServer::RemoteMethod
         (scope, scope.getComponents()[scope.getComponents().size() -1],
          listenerConfig, informerConfig));
    method->setSignalParticipantDestroyed(this->signalParticipantDestroyed);
    (*this->signalParticipantCreated)(method, parent);
    return method;
}

patterns::RemoteServerPtr Factory::createRemoteServer(const Scope&             scope,
                                                      const ParticipantConfig &listenerConfig,
                                                      const ParticipantConfig &informerConfig,
                                                      ParticipantPtr           parent) {
    patterns::RemoteServerPtr server(
        new patterns::RemoteServer(scope, listenerConfig, informerConfig));
    server->setSignalParticipantDestroyed(this->signalParticipantDestroyed);
    (*this->signalParticipantCreated)(server, parent);
    return server;
}

ParticipantConfig Factory::getDefaultParticipantConfig() const {
    boost::recursive_mutex::scoped_lock lock(configMutex);
    return defaultConfig;
}

void Factory::setDefaultParticipantConfig(const ParticipantConfig& config) {
    boost::recursive_mutex::scoped_lock lock(configMutex);
    this->defaultConfig = config;
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
        try {
            connectors.push_back(InPullConnectorPtr(getInPullFactory()
                                                    .createInst(transportIt->getName(),
                                                                prepareConnectorOptions(*transportIt,
                                                                                        DESERIALIZATION,
                                                                                        this->logger))));
        } catch (const exception& e) {
            throw runtime_error(boost::str(boost::format("Error configuring connector `%1%', in-pull: %2%")
                                           % transportIt->getName() % e.what()));
        }
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
        try {
            connectors.push_back(InPushConnectorPtr(getInPushFactory()
                                                    .createInst(transportIt->getName(),
                                                                prepareConnectorOptions(*transportIt,
                                                                                        DESERIALIZATION,
                                                                                        this->logger))));
        } catch (const exception& e) {
            throw runtime_error(boost::str(boost::format("Error configuring connector `%1%', in-push: %2%")
                                           % transportIt->getName() % e.what()));
        }
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
        try {
            connectors.push_back(OutConnectorPtr(getOutFactory()
                                                 .createInst(transportIt->getName(),
                                                             prepareConnectorOptions(*transportIt,
                                                                                     SERIALIZATION,
                                                                                     this->logger))));
        } catch (const exception& e) {
            throw runtime_error(boost::str(boost::format("Error configuring connector `%1%', out: %2%")
                                           % transportIt->getName() % e.what()));
        }
    }
    return connectors;
}

rsc::plugins::ManagerPtr Factory::getPluginManager() const {
    return this->pluginManager;
}

}
