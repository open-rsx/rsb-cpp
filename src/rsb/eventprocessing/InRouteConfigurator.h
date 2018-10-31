/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2011-2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <string>
#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <rsc/runtime/Printable.h>

#include "../ParticipantConfig.h"

#include "rsb/rsbexports.h"

namespace rsb {

class QualityOfServiceSpec;

class Scope;

class Event;
typedef boost::shared_ptr<Event> EventPtr;

class Handler;
typedef boost::shared_ptr<Handler> HandlerPtr;

namespace filter {
class Filter;
typedef boost::shared_ptr<Filter> FilterPtr;
}

namespace transport {
class InConnector;
typedef boost::shared_ptr<InConnector> InConnectorPtr;
}

namespace eventprocessing {

class EventReceivingStrategy;
typedef boost::shared_ptr<EventReceivingStrategy> EventReceivingStrategyPtr;

/**
 * A class responsible of configuring the route that processes
 * incoming events from one or more @ref rsb::transport::InConnector
 * instances.
 *
 * This responsibility includes updates to the transport and filter
 * layers.
 *
 * @author swrede
 * @author jmoringe
 */
class RSB_EXPORT InRouteConfigurator: public virtual rsc::runtime::Printable,
                                      private boost::noncopyable {
public:
    typedef std::set<transport::InConnectorPtr> ConnectorSet;

    InRouteConfigurator(const Scope&             scope,
                        const ParticipantConfig& config);
    virtual ~InRouteConfigurator();

    std::string getClassName() const;
    void printContents(std::ostream& stream) const;

    const std::set<std::string> getTransportURLs() const;

    virtual void activate();
    virtual void deactivate();

    EventReceivingStrategyPtr getEventReceivingStrategy() const;

    ConnectorSet getConnectors();

    void addConnector(transport::InConnectorPtr connector);
    void removeConnector(transport::InConnectorPtr connector);

    void filterAdded(filter::FilterPtr filter);
    void filterRemoved(filter::FilterPtr filter);

    /**
     * Adds a new handler that will be notified about received events.
     *
     * @param handler the handler to add
     * @param wait if set to @c true, this call will return only after the
     *             handler has been completely installed and will receive the
     *             next available event
     */
    void handlerAdded(rsb::HandlerPtr handler, const bool& wait);

    /**
     * Removes a previously registered handle.
     *
     * @param handler handler to remove
     * @param wait if set to @c true, this call will return only after the
     *             handler has been completely removed and will not be notified
     *             anymore
     */
    void handlerRemoved(rsb::HandlerPtr handler, const bool& wait);

    /**
     * Sets the desired error strategy to use.
     *
     * @param strategy the strategy to use
     */
    void setErrorStrategy(const ParticipantConfig::ErrorStrategy& strategy);

    /**
     * Define the desired quality of service specifications for published
     * events.
     *
     * @param specs QoS specification
     * @throw UnsupportedQualityOfServiceException requirements cannot be met
     */
    void setQualityOfServiceSpecs(const QualityOfServiceSpec& specs);
private:

    class Impl;
    boost::scoped_ptr<Impl> d;
};

typedef boost::shared_ptr<InRouteConfigurator> InRouteConfiguratorPtr;

}
}
