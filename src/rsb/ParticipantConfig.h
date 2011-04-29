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

#include <rsc/runtime/Properties.h>

#include "QualityOfServiceSpec.h"
#include "rsb/rsbexports.h"

namespace rsb {

/**
 * A class describing the configuration of Participant instances. This contains
 * QoS settings as well as configuration options for the desired transports.
 *
 * @author jwienke
 */
class RSB_EXPORT ParticipantConfig {
public:

    /**
     * Description of a desired transport. Transport configurations are
     * compared by the name of the transport they describe because one transport
     * can exist only once per participant.
     *
     * @author jwienke
     */
    class Transport: boost::totally_ordered<Transport> {
    public:

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

        bool operator==(const Transport &other) const;
        bool operator<(const Transport &other) const;

    private:

        std::string name;
        rsc::runtime::Properties options;

    };

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
     * Returns the set of desired transports for a participant.
     *
     * @return set of transports identified by strings
     * @note generates a copy. Changing the returned object does not change this
     *       configuration
     */
    std::set<Transport> getTransports() const;

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

    friend std::ostream &operator<<(std::ostream &stream,
            const ParticipantConfig &config);

private:

    QualityOfServiceSpec qosSpec;
    std::set<Transport> transports;
    rsc::runtime::Properties options;

};

RSB_EXPORT std::ostream &operator<<(std::ostream &stream,
        const ParticipantConfig::Transport &transport);
RSB_EXPORT std::ostream &operator<<(std::ostream &stream,
        const ParticipantConfig &config);

}

