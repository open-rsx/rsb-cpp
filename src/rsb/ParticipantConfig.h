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
#include <rsc/misc/uri.h>

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
        /**
         * The set of preferred converters lists (wire-schema, data-type) pairs.
         */
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
        explicit Transport(const std::string& name,
                           bool enabled = true);
        virtual ~Transport();

        /**
         * Returns the name of this transport description.
         */
        std::string getName() const;

        /**
         * Return the set of preferred converters.
         * This list is typically initialized from options. See @a handleOption().
         */
        ConverterNames getConverters() const;

        /**
         * add a preferred converter
         */
        void addConverter(const std::string &wireschema, const std::string &datatype);

        /**
         * add preferred converters
         */
        void addConverters(const ConverterNames& converters);

        /**
         * Returns the specified options for the transport.
         *
         * @return copy of options for the transport
         */
        rsc::runtime::Properties getOptions() const;

        /**
         * Returns the options for the transport.
         *
         * @return mutable reference to options for the transport
         */
        rsc::runtime::Properties& mutableOptions();

        /**
         * Sets the options for the transport.
         *
         * @param options new options replacing all old ones
         */
        void setOptions(const rsc::runtime::Properties& options);

        bool isEnabled() const;

        void setEnabled(bool value);

        bool operator==(const Transport& other) const;
        bool operator<(const Transport& other) const;

        std::string getClassName() const;
        void printContents(std::ostream& stream) const;

        void handleOption(const std::vector<std::string>& key,
                const std::string& value);

        /**
         * Merge in options from uri: host, port, and query's key=value pairs.
         */
        void mergeOptions (const rsc::misc::uri& uri);

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
        EventProcessingStrategy(const std::string& name);

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
        void setName(const std::string& name);

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
        rsc::runtime::Properties& mutableOptions();

        /**
         * Sets the options for the strategy.
         *
         * @param options new options replacing all old ones
         */
        void setOptions(const rsc::runtime::Properties& options);

        void handleOption(const std::vector<std::string>& key,
                          const std::string& value);

        void printContents(std::ostream& stream) const;
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
        ERROR_STRATEGY_LOG,
        /**
         * Uses stderr for printing a message.
         */
        ERROR_STRATEGY_PRINT,
        /**
         * exits the program.
         */
        ERROR_STRATEGY_EXIT
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
     * @return quality of service settings as immutable copy
     */
    QualityOfServiceSpec getQualityOfServiceSpec() const;

    /**
     * Returns mutable quality of service settings.
     *
     * @return reference to QoS settings
     */
    QualityOfServiceSpec& mutableQualityOfServiceSpec();

    /**
     * Sets the desired QoS settings.
     *
     * @param spec new settings
     */
    void setQualityOfServiceSpec(const QualityOfServiceSpec& spec);

    /**
     * Returns the selected error strategy for the configured participant.
     *
     * @return strategy to use
     */
    ErrorStrategy getErrorStrategy() const;

    /**
     * Sets the desired error strategy for the participant.
     */
    void setErrorStrategy(const ErrorStrategy& strategy);

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
    Transport getTransport(const std::string& name) const;

    /**
     * Returns a single configured transport which can be modified in place.
     *
     * @param name name of the transport to get
     * @return reference to the transport
     * @throw rsc::runtime::NoSuchObject no such transport available with the
     *                                   given name
     */
    Transport& mutableTransport(const std::string& name);

    /**
     * Adds a transport to the list of desired transport mechanisms. It will
     * replace a potentially existing transport configuration for the same
     * transport name.
     *
     * @param transport config of the transport
     */
    void addTransport(const Transport& transport);

    /**
     * Removes a transport from the list of desired transports if it was
     * present.
     *
     * @param transport to remove
     */
    void removeTransport(const Transport& transport);

    /**
     * Replaces all existing transports with the ones given here.
     *
     * @param transports set of transports
     */
    void setTransports(const std::set<Transport>& transports);

    /**
     * Configures existing or new transport from uri.
     *
     * If transport is not specified (empty uri scheme), configure
     * all existing transports with the provided options.
     *
     * @param uri uri instance describing transport.
     * @param disableOthers decides whether all other existing transports (except the requested one) should be disabled, i.e. turning addTransport into setTransport.
     */
    void addTransport (const rsc::misc::uri& uri, bool disableOthers=false);

    /**
     * Configures the set of transports according to the given set of uris.
     *
     * An empty uri scheme is not allowed and throws an invalid_argument exception.
     * Override existing transport configurations with specified values.
     * Removes all other existing transports.
     *
     * @param uris set of uri instances describing the desired transport parameters.
     */
    void setTransports (const std::set<rsc::misc::uri>& uris);

    const EventProcessingStrategy& getEventReceivingStrategy() const;

    EventProcessingStrategy& mutableEventReceivingStrategy();


    const EventProcessingStrategy& getEventSendingStrategy() const;

    /**
     * Indicates whether introspection should be enabled for the
     * participant.
     *
     * @return @c true if introspection should be enabled for the
     *         participant, @c false otherwise.
     */
    bool isIntrospectionEnabled() const;

    /**
     * Controls whether introspection should be enabled for the
     * participant.
     *
     * @param newValue If @c true, introspection should be enabled for
     *                 the participant.
     */
    void setIsIntrospectionEnabled(bool newValue);

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
    rsc::runtime::Properties& mutableOptions();

    /**
     * Sets the additional options besides the transport-specific ones.
     *
     * @param options new options replacing all old ones
     */
    void setOptions(const rsc::runtime::Properties& options);

    /**
     * Adds converters to the list of preferred converters for all currently
     * configurred transports.
     * @param converters: set of (wire-schema, data-type) pairs (as strings).
     */
    void addPreferredConverters (const Transport::ConverterNames &converters);

private:
    rsc::logging::LoggerPtr logger;

    QualityOfServiceSpec             qosSpec;
    ErrorStrategy                    errorStrategy;
    std::map<std::string, Transport> transports;
    EventProcessingStrategy          eventReceivingStrategy;
    EventProcessingStrategy          eventSendingStrategy;
    bool                             introspection;
    rsc::runtime::Properties         options;

    void handleOption(const std::vector<std::string>& key,
            const std::string& value);

    void printContents(std::ostream& stream) const;
};

}
