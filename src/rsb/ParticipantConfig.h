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

#pragma once

#include <set>
#include <string>
#include <ostream>

#include <boost/operators.hpp>
#include <boost/filesystem/path.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/config/OptionHandler.h>
#include <rsc/runtime/Properties.h>
#include <rsc/runtime/Printable.h>

#include "QualityOfServiceSpec.h"
#include "rsb/rsbexports.h"

namespace rsb {

/**
 * A class describing the configuration of Participant instances. This contains:
 * - QoS settings (see @ref rsb::QualityOfServiceSpec)
 * - Error handling strategies
 * - Selection of transport mechanism
 *   - Configuration options for selected transports
 *   - Converters associated to selected transports
 *     (see @ref rsb::converter::Converter)
 * - Selection of event processing strategies
 *   - Event receiving strategy (see @ref rsb::eventprocessing::EventReceivingStrategy)
 *   - Event sending strategy (see @ref rsb::eventprocessing::EventSendingStrategy)
 *
 * @author jwienke
 * @author jmoringe
 */
class RSB_EXPORT ParticipantConfig: public rsc::config::OptionHandler,
                                    public rsc::runtime::Printable {
public:

    /**
     * Description of a desired transport. Transport configurations are
     * compared by the name of the transport they describe because one transport
     * can exist only once per participant.
     *
     * @author jwienke
     */
    class RSB_EXPORT Transport: boost::totally_ordered<Transport>,
            public rsc::config::OptionHandler,
            public rsc::runtime::Printable {
    public:
        typedef std::set<std::pair<std::string, std::string> > ConverterNames;

        /**
         * Creates a new transport description for the transport with the given
         * name.
         *
         * @param name name of the transport to describe
         * @param enabled controls whether the transport is used by
         *                default.
         * @throw std::invalid_argument empty name given, a transport cannot
         *                              have an empty name
         */
        explicit Transport(const std::string &name,
                           bool enabled = true);
        virtual ~Transport();

        /**
         * Returns the name of this transport description.
         */
        std::string getName() const;

        ConverterNames getConverters() const;

        /**
         * Returns the specified options for the transport.
         *
         * @return copy of options for the transport
         */
        rsc::runtime::Properties getOptions() const;

        /**
         * Sets the options for the transport.
         *
         * @param options new options replacing all old ones
         */
        void setOptions(const rsc::runtime::Properties &options);

        bool isEnabled() const;

        void setEnabled(bool value);

        bool operator==(const Transport &other) const;
        bool operator<(const Transport &other) const;

        std::string getClassName() const;
        void printContents(std::ostream &stream) const;

        void handleOption(const std::vector<std::string> &key,
                const std::string &value);
    private:
        std::string name;
        ConverterNames converters;
        rsc::runtime::Properties options;
    };

    /**
     * Instances of this class describe the selection and
     * configuration of an event processing strategy.
     *
     * This mechanism is applied to select and configure
     * implementations of @ref eventprocessing::EventSendingStrategy
     * and @ref eventprocessing::EventReceivingStrategy.
     *
     * @author jmoringe
     */
    class RSB_EXPORT EventProcessingStrategy : public rsc::config::OptionHandler,
                                    public rsc::runtime::Printable {
    public:
        EventProcessingStrategy(const std::string &name);

        /**
         * Returns the name of the implementation to be selected.
         *
         * @return The name of the processing strategy implementation
         * that should be used.
         */
        std::string getName() const;

        /**
         * Sets the name of the implementation to be selected.
         *
         * @param name - Name of the processing strategy
         * implementation that should be used.
         */
        void setName(const std::string &name);

        /**
         * Returns the options for the strategy.
         *
         * @return copy of options for the strategy
         */
        rsc::runtime::Properties getOptions() const;

        /**
         * Returns the options for the strategy.
         *
         * @return mutable reference to options for the strategy
         */
        rsc::runtime::Properties &mutableOptions();

        /**
         * Sets the options for the strategy.
         *
         * @param options new options replacing all old ones
         */
        void setOptions(const rsc::runtime::Properties &options);

        void handleOption(const std::vector<std::string> &key,
                          const std::string &value);

        void printContents(std::ostream &stream) const;
    private:
        std::string name;
        rsc::runtime::Properties options;
    };

    /**
     * Possible error handling strategies in user-provided code like event
     * handlers.
     *
     * @author jwienke
     */
    enum ErrorStrategy {
        /**
         * Logs a message using the logging mechanism.
         */
        LOG,
        /**
         * Uses stderr for printing a message.
         */
        PRINT,
        /**
         * exits the program.
         */
        EXIT
    };

    /**
     * Constructs a new empty configuration using the default QoS settings and
     * #LOG as error strategy.
     */
    ParticipantConfig();
    virtual ~ParticipantConfig();

    /**
     * Returns the current settings for QoS.
     *
     * @return quality of service settings
     */
    QualityOfServiceSpec getQualityOfServiceSpec() const;

    /**
     * Sets the desired QoS settings.
     *
     * @param spec new settings
     */
    void setQualityOfServiceSpec(const QualityOfServiceSpec &spec);

    /**
     * Returns the selected error strategy for the configured participant.
     *
     * @return strategy to use
     */
    ErrorStrategy getErrorStrategy() const;

    /**
     * Sets the desired error strategy for the participant.
     */
    void setErrorStrategy(const ErrorStrategy &strategy);

    /**
     * Returns the set of desired transports for a participant.
     *
     * @param includeDisabled If true, include transports that have
     * been disabled via configuration options.
     * @return set of transports identified by strings
     * @note generates a copy. Changing the returned object does not change this
     *       configuration
     */
    std::set<Transport> getTransports(bool includeDisabled = false) const;

    /**
     * Returns an immutable copy of a single configured transport.
     *
     * @param name name of the transport to get
     * @return copy of the transport
     * @throw rsc::runtime::NoSuchObject no such transport available with the
     *                                   given name
     */
    Transport getTransport(const std::string &name) const;

    /**
     * Returns a single configured transport which can be modified in place.
     *
     * @param name name of the transport to get
     * @return reference to the transport
     * @throw rsc::runtime::NoSuchObject no such transport available with the
     *                                   given name
     */
    Transport &mutableTransport(const std::string &name);

    /**
     * Adds a transport to the list of desired transport mechanisms.
     *
     * @param transport config of the transport
     */
    void addTransport(const Transport &transport);

    /**
     * Removes a transport from the list of desired transports if it was
     * present.
     *
     * @param transport to remove
     */
    void removeTransport(const Transport &transport);

    /**
     * Sets all desired transports in this configuration.
     *
     * @param transports set of transports
     */
    void setTransports(const std::set<Transport> &transports);

    const EventProcessingStrategy &getEventReceivingStrategy() const;

    EventProcessingStrategy &mutableEventReceivingStrategy();


    const EventProcessingStrategy &getEventSendingStrategy() const;

    /**
     * Returns additional options besides the transport-specific ones.
     *
     * @return copy of additional options
     */
    rsc::runtime::Properties getOptions() const;

    /**
     * Returns a mutable reference to the freestyle options in this
     * configuration.
     *
     * @return mutable reference to additional options
     */
    rsc::runtime::Properties &mutableOptions();

    /**
     * Sets the additional options besides the transport-specific ones.
     *
     * @param options new options replacing all old ones
     */
    void setOptions(const rsc::runtime::Properties &options);

    /**
     * Obtain configuration options from the configuration file @a
     * path, store them in a @ref ParticipantConfig object and return
     * it.
     *
     * A simple configuration file may look like this:
     * @verbatim
     [transport.spread]
     host = azurit
     port = 5301
     # A comment
     @endverbatim
     *
     * @param path The path of a file from which configuration options
     * should be loaded.
     * @param defaults A @ref ParticipantConfig object that supplies
     * values for configuration options which are not present in @a
     * path.
     * @return A new @ref ParticipantConfig object reflecting the
     * contents of @a path possibly merged with options from @a
     * defaults.
     *
     * @see fromEnvironment, fromConfiguration
     */
    static ParticipantConfig fromFile(const boost::filesystem::path &path,
            const ParticipantConfig &defaults = ParticipantConfig());

    /**
     * Obtain configuration options from environment variables, store
     * them in a @ref ParticipantConfig object and return
     * it. Environment variable names are mapped to RSB option names
     * as illustrated in the following example:
     *
     * @verbatim
     RSB_TRANSPORT_SPREAD_PORT -> transport spread port
     @endverbatim
     *
     * @param defaults A @ref ParticipantConfig object that supplies
     * values for configuration options for which no environment
     * variables are found.
     * @return A @ref ParticipantConfig object that contains the
     * merged configuration options from @a defaults and relevant
     * environment variables.
     *
     * @see fromFile, fromConfiguration
     */
    static ParticipantConfig fromEnvironment(const ParticipantConfig &defaults =
            ParticipantConfig());

    /**
     * Obtain configuration options from multiple sources, store them
     * in a @ref ParticipantConfig object and return it. The following
     * sources of configuration information will be consulted:
     *
     * -# ~/.config/rsb.conf
     * -# \$(PWD)/rsb.conf
     * -# Environment Variables
     *
     * @param defaults A @ref ParticipantConfig object the options of
     * which should be used as defaults.
     * @return A @ref ParticipantConfig object that contains the
     * merged configuration options from the sources mentioned above.
     *
     * @see fromFile, fromEnvironment
     */
    static ParticipantConfig fromConfiguration(
            const ParticipantConfig &defaults = ParticipantConfig());
private:
    rsc::logging::LoggerPtr logger;

    QualityOfServiceSpec             qosSpec;
    ErrorStrategy                    errorStrategy;
    std::map<std::string, Transport> transports;
    EventProcessingStrategy          eventReceivingStrategy;
    EventProcessingStrategy          eventSendingStrategy;
    rsc::runtime::Properties         options;

    void handleOption(const std::vector<std::string> &key,
            const std::string &value);

    void printContents(std::ostream &stream) const;
};

}
