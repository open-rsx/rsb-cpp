/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2010-2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "../ParticipantConfig.h"
#include "../Handler.h"

#include "rsb/rsbexports.h"

namespace rsb {

class Event;
typedef boost::shared_ptr<Event> EventPtr;

namespace filter {
class Filter;
typedef boost::shared_ptr<Filter> FilterPtr;
}

namespace eventprocessing {

/**
 * Implementations of this interface organize the receiving of events
 * via @ref rsb::transport::InConnector s.
 *
 * A list of handlers is maintained and dispatching of events is done
 * by calling each handler.
 *
 * @author swrede
 * @author jmoringe
 */
class RSB_EXPORT EventReceivingStrategy: public Handler,
                                         private boost::noncopyable {
public:
    virtual ~EventReceivingStrategy();

    virtual void addFilter(filter::FilterPtr filter) = 0;
    virtual void removeFilter(filter::FilterPtr filter) = 0;

    /**
     * Adds a new handler that will be notified about new events.
     *
     * @param handler handler to add
     * @param wait if set to @c true, this method must only return after the
     *             handler has been install completely so that the next event
     *             will be delivered to it
     */
    virtual void addHandler(rsb::HandlerPtr handler, const bool& wait) = 0;

    /**
     * Removes a handler that will will then not be notified anymore.
     *
     * @param handler handler to remove
     * @param wait if set to @c true, this method must only return after the
     *             handler has been removed completely and will not receive
     *             any more notifications
     */
    virtual void removeHandler(rsb::HandlerPtr handler, const bool& wait) = 0;

    /**
     * Defines the strategy to use for handling dispatching errors to
     * the client handler.
     *
     * @param strategy the new strategy to use
     */
    virtual void setHandlerErrorStrategy(
        const ParticipantConfig::ErrorStrategy& strategy) = 0;

    /**
     * Dispatches the event to the listener.
     *
     * @param event the event to dispatch
     */
    virtual void handle(EventPtr event) = 0;
};

typedef boost::shared_ptr<EventReceivingStrategy> EventReceivingStrategyPtr;

}
}
