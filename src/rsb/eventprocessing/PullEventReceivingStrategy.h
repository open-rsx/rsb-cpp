/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "EventReceivingStrategy.h"

namespace rsb {

class Event;
typedef boost::shared_ptr<Event> EventPtr;

namespace filter {
class Filter;
typedef boost::shared_ptr<Filter> FilterPtr;
}

namespace transport {
class InPullConnector;
typedef boost::shared_ptr<InPullConnector> InPullConnectorPtr;
}

namespace eventprocessing {

/**
 * Instances of this class retrieve @ref Event s from @ref
 * transport::Connector s when explicitly asked by a client (which
 * usually is a @ref Participant ). The Retrieval works (roughly) as
 * follows:
 *
 * -# The client calls the emit method of the @c
 *    PullEventReceivingStrategy instance
 * -# The instance calls the emit methods of its connectors
 * -# A connector calls the handle method of the instance
 * -# The instance performs event filtering
 * -# If the event has not been discarded during filtering, it is
 *    returned from emit
 *
 * @author jmoringe
 */
class RSB_EXPORT PullEventReceivingStrategy: public EventReceivingStrategy {
public:
    PullEventReceivingStrategy(const std::set<transport::InPullConnectorPtr>& connectors);
    virtual ~PullEventReceivingStrategy();

    virtual void addFilter(filter::FilterPtr filter);
    virtual void removeFilter(filter::FilterPtr filter);

    /**
     * Retrieve an @ref Event from the connectors and return it.
     * @param block Controls whether the call should block until an
     * event becomes available if that is not the case immediately.
     *
     * @return A pointer to the received event.
     */
    EventPtr raiseEvent(bool block);

    /**
     * This method is called by @ref rsb::transport::InPullConnector s
     * in response to calls to their emit methods.
     *
     * @param event The @ref Event that should be filtered and
     * potentially be delivered to the client.
     */
    void handle(EventPtr event);
private:

    class Impl;
    boost::scoped_ptr<Impl> d;

    std::string getClassName() const;
    void printContents(std::ostream& stream) const;
};

typedef boost::shared_ptr<PullEventReceivingStrategy> PullEventReceivingStrategyPtr;

}
}
