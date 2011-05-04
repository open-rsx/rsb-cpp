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
 * A class describing the configuration of Participant instances. This contains
 * QoS settings as well as configuration options for the desired transports.
 *
 * @author jwienke
 */
class RSB_EXPORT ParticipantConfig: public rsc::config::OptionHandler {
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
        typedef std::set< std::pair<std::string, std::string> > ConverterNames;

        /**
         * Creates a new transport description for the transport with the given
         * name.
         *
         * @param name name of the transport to describe
         * @throw std::invalid_argument empty name given, a transport cannot
         *                              have an empty name
         */
        explicit Transport(const std::string &name);
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

    Transport getTransport(const std::string &name) const;

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

    /**
     * Returns additional options besides the transport-specific ones.
     *
     * @return copy of additional options
     */
    rsc::runtime::Properties getOptions() const;

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
     host = azurit # default type is string
     port = <uint>5301 # types can be specified in angle brackets
     # A comment
     @endverbatim
     *
     * @param path File of path
     * @param defaults  defaults
     * @return
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

    friend RSB_EXPORT std::ostream &operator<<(std::ostream &stream,
            const ParticipantConfig &config);

private:
    rsc::logging::LoggerPtr logger;
    QualityOfServiceSpec qosSpec;
    ErrorStrategy errorStrategy;
    std::map<std::string, Transport> transports;
    rsc::runtime::Properties options;

    void handleOption(const std::vector<std::string> &key,
            const std::string &value);
};

RSB_EXPORT std::ostream &operator<<(std::ostream &stream,
        const ParticipantConfig &config);

}
