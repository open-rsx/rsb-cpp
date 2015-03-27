/* ============================================================
 *
 * This file is part of the RSB project.
 *
 * Copyright (C) 2014, 2015 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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
 * ============================================================  */

#include <boost/thread/mutex.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/plugins/Provider.h>

#include <../ParticipantConfig.h>
#include <../Factory.h>

#include <../converter/Repository.h>
#include <../converter/VoidConverter.h>
#include <../converter/StringConverter.h>
#include <../converter/ProtocolBufferConverter.h>

#include <rsb/protocol/introspection/Hello.pb.h>
#include <rsb/protocol/introspection/Bye.pb.h>

#include "IntrospectionSender.h"

// Plugin-level helper stuff

namespace {

template <typename WireType>
void addConverter(rsb::converter::UnambiguousConverterMap<WireType>& selection,
                  rsb::converter::Converter<WireType>*               converter,
                  bool                                               forSerialization) {
    if (forSerialization) {
        selection.addConverter(converter->getDataType(),
                               typename rsb::converter::Converter<WireType>::Ptr(converter));
    } else {
        selection.addConverter(converter->getWireSchema(),
                               typename rsb::converter::Converter<WireType>::Ptr(converter));
    }
}

rsb::ParticipantConfig createConfig(const rsb::ParticipantConfig& defaultConfig,
                                    bool forSerialization) {
    rsb::ParticipantConfig config = defaultConfig;

    boost::shared_ptr< rsb::converter::UnambiguousConverterMap<std::string> > converterSelection
        (new rsb::converter::UnambiguousConverterMap<std::string>());
    addConverter(*converterSelection, new rsb::converter::VoidConverter(),
                 forSerialization);
    addConverter(*converterSelection, new rsb::converter::StringConverter(),
                 forSerialization);
    addConverter(*converterSelection,
                 new rsb::converter::ProtocolBufferConverter<rsb::protocol::introspection::Hello>(),
                 forSerialization);
    addConverter(*converterSelection,
                 new rsb::converter::ProtocolBufferConverter<rsb::protocol::introspection::Bye>(),
                 forSerialization);

    std::set<rsb::ParticipantConfig::Transport> transports
        = config.getTransports();
    for (std::set<rsb::ParticipantConfig::Transport>::const_iterator it
             = transports.begin(); it != transports.end(); ++it) {
        rsb::ParticipantConfig::Transport& transport
            = config.mutableTransport(it->getName());
        rsc::runtime::Properties options = transport.getOptions();
        options["converters"] = rsb::converter::ConverterSelectionStrategy<std::string>::Ptr(converterSelection);
        transport.setOptions(options);
    }

    return config;
}

class IntrospectionParticipantObserver : public rsc::config::OptionHandler {
public:
    IntrospectionParticipantObserver()
        : logger(rsc::logging::Logger::getLogger(
                     "rsb.introspection.IntrospectionParticipantObserver")) {
    }

    void handleOption(const std::vector<std::string>& key,
                      const std::string& value) {
        if (key.size() == 2 && key[0] == "introspection"
            && key[1] == "displayname") {
            processDisplayName.reset(new std::string(value));
        }
    }

    void handleParticipantCreated(rsb::ParticipantPtr participant,
                                  rsb::ParticipantPtr parent) {
        // Do not print out the participant directly since parts of might not be
        // constructed, which could be accessed by the participan's printing method
        RSCDEBUG(logger,
                 "Was notified of created participant " << participant->getId() <<
                 " at scope " << *participant->getScope());

        if (participant->getScope()->isSubScopeOf("/__rsb/introspection")
            || !participant->getConfig().isIntrospectionEnabled()) {
            RSCDEBUG(logger, "Ignoring created participant " << participant);
            return;
        }

        {
            boost::mutex::scoped_lock lock(senderMutex);

            if (!sender) {
                RSCDEBUG(logger, "Creating introspection sender");
                configSerialization   = createConfig(rsb::getFactory().getDefaultParticipantConfig(), true);
                configDeserialization = createConfig(rsb::getFactory().getDefaultParticipantConfig(), false);
                sender.reset(
                    new rsb::introspection::IntrospectionSender(
                        processDisplayName,
                        configDeserialization, configSerialization));
            }

            sender->addParticipant(participant, parent);
        }
    }

    void handleParticipantDestroyed(rsb::Participant* participant) {
        // Do not print out the participant directly since parts of it are already
        // destroyed, which could be accessed by the participan's printing method
        RSCDEBUG(logger,
                 "Was notified of destroyed participant " << participant->getId() <<
                 " at scope " << *participant->getScope());

        // Exit early for "uninteresting" participants (e.g. disabled
        // introspection or internal to the introspection machinery).
        if (participant->getScope()->isSubScopeOf("/__rsb/introspection")
            || !participant->getConfig().isIntrospectionEnabled()) {
            RSCDEBUG(logger, "Ignoring destroyed participant " << participant);
            return;
        }

        // "Ordinary" participant: not internal and introspection is
        // enabled. Remove the participant from the introspection sender,
        // but only if the sender exists.
        //
        // The following lock must not extend to the conditional exit
        // above since otherwise sender.reset() would cause recursive
        // entry into handleParticipantDestroyed and thus a recursive lock
        // attempt.
        {
            boost::mutex::scoped_lock lock(senderMutex);

            if (!sender) {
                RSCDEBUG(logger, "Ignoring destroyed participant " << participant);
                return;
            }

            if (!sender->removeParticipant(*participant)) {
                sender.reset();
            }
        }
    }
private:
    rsc::logging::LoggerPtr logger;

    rsb::ParticipantConfig configSerialization;
    rsb::ParticipantConfig configDeserialization;

    boost::shared_ptr<std::string> processDisplayName;

    rsb::introspection::IntrospectionSenderPtr sender;
    boost::mutex senderMutex;
};

typedef boost::shared_ptr<IntrospectionParticipantObserver> IntrospectionParticipantObserverPtr;

IntrospectionParticipantObserverPtr observer;

boost::signals2::connection participantCreatedConnection;
boost::signals2::connection participantDestroyedConnection;

// External plugin interface

extern "C" {

    void RSC_PLUGIN_INIT_SYMBOL() {
        // Register converters.
        rsc::logging::LoggerPtr logger = rsc::logging::Logger::getLogger("rsb.introspection");
        RSCDEBUG(logger, "Registering converters for introspection types");

        rsb::converter::converterRepository<std::string>()
            ->registerConverter(rsb::converter::ProtocolBufferConverter<rsb::protocol::introspection::Hello>::Ptr
                                (new rsb::converter::ProtocolBufferConverter<rsb::protocol::introspection::Hello>()));
        rsb::converter::converterRepository<std::string>()
            ->registerConverter(rsb::converter::ProtocolBufferConverter<rsb::protocol::introspection::Bye>::Ptr
                                (new rsb::converter::ProtocolBufferConverter<rsb::protocol::introspection::Bye>()));

        observer.reset(new IntrospectionParticipantObserver());

        // Process configuration to obtain value of the
        // introspection.displayname option.
        rsb::Factory::provideConfigOptions(*observer);

        // Register participant creation hook.
        RSCDEBUG(logger, "Registering participant creation and destruction hooks");

        participantCreatedConnection
            = rsb::getFactory().getSignalParticipantCreated()
            ->connect(boost::bind(&IntrospectionParticipantObserver::handleParticipantCreated, observer, _1, _2));
        participantDestroyedConnection
            = rsb::getFactory().getSignalParticipantDestroyed()
            ->connect(boost::bind(&IntrospectionParticipantObserver::handleParticipantDestroyed, observer, _1));
    }

    void RSC_PLUGIN_SHUTDOWN_SYMBOL() {
        rsc::logging::LoggerPtr logger = rsc::logging::Logger::getLogger("rsb.introspection");

        RSCDEBUG(logger, "Unregistering participant creation and destruction hooks");

        participantCreatedConnection.disconnect();
        participantDestroyedConnection.disconnect();

        RSCDEBUG(logger, "Destroying introspection sender");

        observer.reset();
    }

}

}
