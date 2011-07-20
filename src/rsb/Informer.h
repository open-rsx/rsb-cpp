/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include <string>

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include <rsc/runtime/TypeStringTools.h>
#include <rsc/logging/Logger.h>

#include "Event.h"
#include "Participant.h"
#include "eventprocessing/OutRouteConfigurator.h"
#include "transport/Connector.h"
#include "transport/Factory.h"
#include "QualityOfServiceSpec.h"

namespace rsb {

/**
 * A informer to publish data of a specified default type expressed through the
 * template parameter. All data in RSB is maintained as shared pointers to avoid
 * unnecessary copy operations. Typedefs simplify the use of the pointer types.
 *
 * The basic usage pattern is explained with this example code:
 * @code
 * Informer<string>::Ptr informer(new Informer<string>(Scope("/example/informer")));
 * Informer<string>::DataPtr s(new string("blub"));
 * informer->publish(s);
 * @endcode
 *
 * @author swrede
 * @tparam T default data type to send by this informer
 */
template<class T>
class Informer: public Participant {
public:

    /**
     * Shared pointer type for this informer.
     */
    typedef boost::shared_ptr<Informer<T> > Ptr;

    /**
     * Shared pointer type for the default data published by this informer.
     */
    typedef boost::shared_ptr<T> DataPtr;

    /**
     * Constructs a new informer.
     *
     * @param connectors A list of connectors the informer should use
     *                   to connect to the bus
     * @param scope the scope under which the data are published
     * @param config the config that was used to setup this informer
     * @param type string describing the default type of data sent by this
     *             informer. It is used to find a converter that can convert
     *             these data to the port
     *
     * @note This constructor is exposed for unit tests and such. Use
     * @ref Factory::createInformer instead of calling this directly.
     */
    Informer(const std::vector<transport::OutConnectorPtr> &connectors,
            const Scope &scope, const ParticipantConfig &config,
            const std::string &type = rsc::runtime::typeName<T>()) :
        Participant(scope, config),
        logger(rsc::logging::Logger::getLogger("rsb.Informer")),
        defaultType(type),
        currentSequenceNumber(0) {
        // TODO evaluate configuration
        this->configurator.reset(new eventprocessing::OutRouteConfigurator());
        for (std::vector<transport::OutConnectorPtr>::const_iterator it =
                connectors.begin(); it != connectors.end(); ++it) {
            this->configurator->addConnector(*it);
        }

        this->configurator->activate();
    }

    virtual ~Informer() {
    }

    std::string getClassName() const {
        return "Informer";
    }

    void printContents(std::ostream &stream) const {
        Participant::printContents(stream);
        stream << ", type = " << defaultType;
    }

    /**
     * Return the event payload type of this Informer.
     *
     * @return A string designating the event payload type of this
     * Informer.
     */
    std::string getType() const {
        return this->defaultType;
    }

    /**
     * Defines the desired quality of service settings for this informers.
     *
     * @param specs QoS specs
     * @throw UnsupportedQualityOfServiceException requirements cannot be met
     */
    void setQualityOfSerivceSpecs(const QualityOfServiceSpec &specs) {
        configurator->setQualityOfServiceSpecs(specs);
    }

    /**
     * Publishes the given data to the Informer's scope.
     *
     * @param data Pointer to the data to send.
     * @return A boost::shared_ptr to the @ref rsb::Event object that has been
     *         implicitly created.
     */
    EventPtr publish(boost::shared_ptr<T> data) {
        VoidPtr p = boost::static_pointer_cast<void>(data);
        return publish(p, defaultType);
    }

    /**
     * Publishes the given data to the Informer's scope.
     *
     * @param data Pointer to the data to send.
     * @param type string which defines the type of the data. I.e. "string"
     *        for strings.
     * @return A boost::shared_ptr to the @ref rsb::Event object
     *         that has been implicitly created.
     */
    template<class T1>
    EventPtr publish(boost::shared_ptr<T1> data, std::string type) {
        VoidPtr p = boost::static_pointer_cast<void>(data);
        return publish(p, type);
    }

    /**
     * Publishes the given event to the Informer's scope with the ability to
     * define additional meta data.
     *
     * @param event The event to publish.
     * @return modified Event instance
     */
    EventPtr publish(EventPtr event) {
        RSCDEBUG(logger, "Publishing event");
        checkedPublish(event);
        return event;
    }

    /**
     * Publishes the given data to the Informer's scope.
     *
     * @param data Pointer to the data to send.
     * @param type Type of
     * @return A boost::shared_ptr to the @ref rsb::Event object
     *         that has been implicitly created.
     */
    EventPtr publish(VoidPtr data, const std::string &type) {
        EventPtr event(new Event());
        event->setData(data);
        event->setScope(getScope());
        event->setType(type);
        RSCDEBUG(logger, "Publishing event");
        checkedPublish(event);
        return event;
    }

private:

    void checkedPublish(EventPtr event) {
        if (event->getType().empty()) {
            throw std::invalid_argument(boost::str(boost::format("Event type cannot be empty: %1%")
                                                   % event));
        }
        // Check event type against informer's declared type.
        if (event->getType() != getType()) {
            throw std::invalid_argument(boost::str(boost::format("Specified event type %1% does not match listener type %2%.")
                                                   % event->getType() % getType()));
        }
        // Check event scope against informer's declared scope.
        if (event->getScope() != getScope()) {
            throw std::invalid_argument(boost::str(boost::format("Specified event scope %1% does not match listener scope %2%.")
                                                   % event->getScope() % getScope()));
        }
        event->setSequenceNumber(nextSequenceNumber());
        event->mutableMetaData().setSenderId(getId());
        configurator->publish(event);
    }

    boost::uint32_t nextSequenceNumber() {
        return ++this->currentSequenceNumber; /** TODO(jmoringe): needs atomic increment */
    }

    rsc::logging::LoggerPtr logger;
    std::string defaultType;
    eventprocessing::OutRouteConfiguratorPtr configurator;
    boost::uint32_t currentSequenceNumber;
};

}
