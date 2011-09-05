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

#include "rsb/rsbexports.h"

#include "Event.h"
#include "QualityOfServiceSpec.h"
#include "Participant.h"

#include "eventprocessing/OutRouteConfigurator.h"

#include "transport/Connector.h"

namespace rsb {

/**
 * A tag type for constructing @ref Informer instances that can
 * publish data of arbitrary types.
 *
 * Usage example:
 * @code
 * Factory::getInstance().createInformer<AnyType>(Scope("/scope"));
 * @endcode
 */
class AnyType {
};

namespace detail {

template<typename T>
struct TypeName {
    std::string operator()() const {
        return rsc::runtime::typeName<T>();
    }
};

template<>
struct TypeName<AnyType> {
    std::string operator()() const {
        return "";
    }
};

}

/**
 * A informer to publish data. All data in RSB is maintained as shared
 * pointers to avoid unnecessary copy operations. Typedefs simplify
 * the use of the pointer types.
 *
 * The basic usage pattern is explained with this example code:
 * @code
 * InformerBasePtr informer = Factory::getInstance().createInformerBase(Scope("/example/informer"));
 * typename InformerBase::DataPtr<string>::type s(new string("blub"));
 * informer->publish(s);
 * @endcode
 *
 * @author swrede
 * @author jmoringe
 */
class RSB_EXPORT InformerBase: public Participant {
public:
    template<typename T>
    struct DataPtr {
        typedef boost::shared_ptr<T> type;
    };

    InformerBase(const std::vector<transport::OutConnectorPtr> &connectors,
            const Scope &scope, const ParticipantConfig &config,
            const std::string &defaultType);

    virtual ~InformerBase();

    void printContents(std::ostream &stream) const;

    /**
     * Return the event payload type of this Informer.
     *
     * @return A string designating the event payload type of this
     *         Informer.
     */
    std::string getType() const;

    /**
     * Defines the desired quality of service settings for this informers.
     *
     * @param specs QoS specs
     * @throw UnsupportedQualityOfServiceException requirements cannot be met
     */
    void setQualityOfSerivceSpecs(const QualityOfServiceSpec &specs);

    /**
     * Published @a data in the channel in which the informer
     * participates.
     *
     * @tparam T1 The type of @a data. The value of this parameter is
     *            used infer the value of @a type.
     * @param data Pointer to the data that should be sent. Arbitrary
     *             pointer types are accepted at compile time, but may
     *             lead to type or conversion errors at runtime.
     * @param type A string indicating the type of @a
     *             data. I.e. "std::string" for @ref std::string
     *             objects. If omitted, the type of @a data is
     *             inferred from @a T1.
     * @return A boost::shared_ptr to the @ref rsb::Event object that
     *         has been implicitly created.
     * @throw std::invalid_argument If @a T1 or @a type is
     *                              incompatible with the actual type
     *                              of the informer.
     */
    template<class T1>
    EventPtr publish(boost::shared_ptr<T1> data,
            std::string type = rsc::runtime::typeName<T1>()) {
        VoidPtr p = boost::static_pointer_cast<void>(data);
        return publish(p, type);
    }

    template<class T1>
    EventPtr uncheckedPublish(boost::shared_ptr<T1> data,
            const std::string &type = rsc::runtime::typeName<T1>()) {
        VoidPtr p = boost::static_pointer_cast<void>(data);
        return uncheckedPublish(p, type);
    }

    /**
     * Publishes @a data to the Informer's scope.
     *
     * @param data Pointer to the data to send.
     * @param type A string indicating the type of @a
     *             data. I.e. "std::string" for @ref std::string
     *             objects.
     * @return A boost::shared_ptr to the @ref rsb::Event object
     *         that has been implicitly created.
     * @throw std::invalid_argument If @a type is incompatible with
     *                              the actual type of the informer.
     */
    EventPtr publish(VoidPtr data, const std::string &type);
    EventPtr uncheckedPublish(VoidPtr data, const std::string &type);

    /**
     * Publishes the @a event to the Informer's scope with the ability
     * to define additional meta data.
     *
     * @param event The event to publish.
     * @return modified @a event.
     * @throw std::invalid_argument If the type of the payload of @a
     *                              event is incompatible with the
     *                              actual type of the informer or if
     *                              the scope of @a event is not a
     *                              subscope of the scope of the
     *                              informer.
     */
    EventPtr publish(EventPtr event);

protected:
    void checkedPublish(EventPtr event);
    void uncheckedPublish(EventPtr event);

    boost::uint32_t nextSequenceNumber();

    std::string defaultType;
    eventprocessing::OutRouteConfiguratorPtr configurator;
    boost::uint32_t currentSequenceNumber;
private:
    rsc::logging::LoggerPtr logger;
};

typedef boost::shared_ptr<InformerBase> InformerBasePtr;

/**
 * A informer to publish data of a specified type expressed through
 * the template parameter. All data in RSB is maintained as shared
 * pointers to avoid unnecessary copy operations. Typedefs simplify
 * the use of the pointer types.
 *
 * The basic usage pattern is explained with this example code:
 * @code
 * Informer<string>::Ptr informer = Factory::getInstance().createInformer<string>(Scope("/example/informer"));
 * Informer<string>::DataPtr s(new string("blub"));
 * informer->publish(s);
 * @endcode
 *
 * @author swrede
 * @tparam T Data type to send by this informer.
 */
template<class T>
class Informer: public InformerBase {
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
     * @param type string describing the type of data sent by this
     *             informer. The emtpy string indicates that data of
     *             arbitry type can be sent through this informer.
     *
     * @note This constructor is exposed for unit tests and such. Use
     * @ref Factory::createInformer instead of calling this directly.
     */
    Informer(const std::vector<transport::OutConnectorPtr> &connectors,
            const Scope &scope, const ParticipantConfig &config,
            const std::string &type = detail::TypeName<T>()()) :
        InformerBase(connectors, scope, config, type),
                logger(rsc::logging::Logger::getLogger(getClassName())) {
    }

    virtual ~Informer() {
    }

    std::string getClassName() const {
        return rsc::runtime::typeName<Informer<T> >();
    }

    /**
     * Publishes @a data to the Informer's scope.
     *
     * @param data Pointer to the data to send.
     * @return A boost::shared_ptr to the @ref rsb::Event object that
     *         has been implicitly created.
     */
    EventPtr publish(boost::shared_ptr<T> data) {
        VoidPtr p = boost::static_pointer_cast<void>(data);
        return InformerBase::publish(p, this->getType());
    }

    template<class T1>
    EventPtr publish(boost::shared_ptr<T1> data,
            std::string type = rsc::runtime::typeName(typeid(T1))) {
        return InformerBase::publish(data, type);
    }

    EventPtr publish(EventPtr event) {
        return InformerBase::publish(event);
    }
private:
    rsc::logging::LoggerPtr logger;
};

}
