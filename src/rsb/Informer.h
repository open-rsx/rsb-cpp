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
 *
 * @todo check thread-safety, e.g. setting router to active and setting the
 *       passive flag must be atomic
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
     */
    Informer(const std::vector<transport::OutConnectorPtr> &connectors,
            const Scope &scope, const ParticipantConfig &config,
            const std::string &type = rsc::runtime::typeName<T>()) :
        Participant(scope, config),
                logger(rsc::logging::Logger::getLogger("rsb.Informer")),
                defaultType(type) {
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
     */
    void publish(boost::shared_ptr<T> data) {
        VoidPtr p = boost::static_pointer_cast<void>(data);
        publish(p, defaultType);
    }

    /**
     * Publishes the given data to the Informer's scope.
     *
     * @param data Pointer to the data to send.
     * @param type string which defines the type of the data. I.e. "string"
     *        for strings.
     */
    template<class T1>
    void publish(boost::shared_ptr<T1> data, std::string type) {
        VoidPtr p = boost::static_pointer_cast<void>(data);
        publish(p, type);
    }

    /**
     * Publishes the given event to the Informer's scope with the ability to
     * define additional meta data.
     *
     * @param event the event to publish.
     */
    void publish(EventPtr event) {
        event->setScope(getScope());
        RSCDEBUG(logger, "Publishing event");
        checkedPublish(event);
    }

    void publish(VoidPtr p, const std::string &type) {
        EventPtr e(new Event());
        e->setData(p);
        e->setScope(getScope());
        e->setType(type);
        RSCDEBUG(logger, "Publishing event");
        checkedPublish(e);
    }

private:

    void checkedPublish(EventPtr event) {
        if (event->getType().empty()) {
            std::stringstream s;
            s << "Event type cannot be empty: " << event;
            throw std::invalid_argument(s.str());
        }
        configurator->publish(event);
    }

    rsc::logging::LoggerPtr logger;
    std::string defaultType;
    eventprocessing::OutRouteConfiguratorPtr configurator;
};

}
