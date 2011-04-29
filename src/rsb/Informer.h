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

#include <rsc/logging/Logger.h>

#include "Event.h"
#include "Participant.h"
#include "eventprocessing/Router.h"
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
 * Informer<string>::Ptr informer(new Informer<string>("rsb://example/informer","string"));
 * Informer<string>::DataPtr s(new string("blub"));
 * informer->publish(s);
 * @endcode
 *
 * @author swrede
 * @tparam T default data type to send by this informer
 *
 * @todo check thread-safety, e.g. setting router to active and setting the
 *       passive flag must be atomic
 * @todo does it make sense that informers are copyable?
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
     * @param scope the scope under which the data are published
     * @param type string describing the default type of data sent by this
     *             informer. It is used to find a converter that can convert
     *             these data to the port
     */
    Informer(const Scope &scope, const std::string &type) :
        Participant(scope), logger(rsc::logging::Logger::getLogger(
                "rsb.Informer." + scope.toString())), passive(false),
                defaultType(type) {
        // TODO evaluate configuration
        router = eventprocessing::RouterPtr(new eventprocessing::Router("", "spread"));
        activate();
    }

    Informer(const std::string &out, const Scope &scope, const std::string &type) :
        Participant(scope), logger(rsc::logging::Logger::getLogger(
                "rsb.Informer")), passive(false), defaultType(type) {
        // TODO evaluate configuration
        router = eventprocessing::RouterPtr(new eventprocessing::Router("", out));
        activate();
    }

    virtual ~Informer() {
    }

    /**
     * Defines the desired quality of service settings for this informers.
     *
     * @param specs QoS specs
     * @throw UnsupportedQualityOfServiceException requirements cannot be met
     */
    void setQualityOfSerivceSpecs(const QualityOfServiceSpec &specs) {
        router->setQualityOfServiceSpecs(specs);
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
     * @todo assumption is that data and type field already set externally
     *       throw exception if not the case
     */
    void publish(EventPtr event) {
        // TODO Check that exception is thrown if no converter available!
        event->setScope(getScope());
        RSCDEBUG(logger, "Publishing event");
        router->publish(event);
    }

    /**
     * Activates the Informer and therefore the Router. Is considered being in
     * active mode afterwards.
     */
    void activate() {
        router->activate();
        passive = false;
    }

    /**
     * Deactivates the Informer and therefore the Router. Is considered being
     * in passive mode afterwards.
     */
    void deactivate() {
        if (!passive) {
            router->deactivate();
        }
        passive = true;
    }

    void publish(VoidPtr p, const std::string &type) {
        EventPtr e(new Event());
        e->setData(p);
        e->setScope(getScope());
        // TODO Check that exception is thrown if no converter available!
        e->setType(type);
        RSCDEBUG(logger, "Publishing event");
        router->publish(e);
    }

private:
    rsc::logging::LoggerPtr logger;
    volatile bool passive;
    std::string defaultType;
    eventprocessing::RouterPtr router;

};

template<typename Ch, typename Tr, typename T>
std::basic_ostream<Ch, Tr>&
operator<<(std::basic_ostream<Ch, Tr> &stream, const Informer<T> &informer) {
    stream << "Informer[id=" << informer.getUUID() << "]";
    return stream;
}

}
