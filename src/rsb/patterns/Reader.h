/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2014, 2015, 2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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
#include <boost/enable_shared_from_this.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/threading/SynchronizedQueue.h>

#include "../Scope.h"
#include "../ParticipantConfig.h"
#include "../Event.h"
#include "../Participant.h"
#include "../Listener.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace patterns {

/**
 * A Reader receives and synchronously delivers events.
 *
 * In contrast to the @ref Listener participant, a Reader delivers
 * events using a pull-style interface: the read method has to be
 * called in order to receive events. This method may block (depending
 * on the @a block parameter) until an event becomes available. The
 * received event, if any, is returned to the caller.
 *
 * Usage example:
 * @code
 * rsb::patterns::ReaderPtr reader
 *         = rsb::getFactory().createReader("/example/informer");
 * reader->read();
 * @endcode
 *
 * @author jmoringe
 */
class RSB_EXPORT Reader: public Participant,
                         public boost::enable_shared_from_this<Reader> {
public:
    /**
     * Constructs a new reader object attached to @a scope.
     *
     * @param scope the scope from which data is received.
     * @param config the configuration for this reader
     *
     * @note This constructor is exposed for unit tests and such. Use
     * @ref Factory::createReader instead of calling this directly.
     */
    Reader(const Scope&             scope,
           const ParticipantConfig& config);

    virtual ~Reader();

    // Overrides method in Participant.
    virtual std::string getKind() const;

    // Overrides method in Participant.
    virtual const std::set<std::string> getTransportURLs() const;

    /**
     * Reads one event from the bus. If @a block is true, blocks until
     * an event is available. If @a block is false and no event is
     * available, an empty @ref EventPtr is returned.
     *
     * @param block Controls whether the call should block and wait
     * for an event if no event is immediately available.
     * @return A @ref boost::shared_ptr to the received event or an
     * empty @ref boost::shared_ptr if no event was available and @a
     * block was false.
     * @throw RSBError when a communication error occurs while
     * receiving or decoding the event.
     */
    EventPtr read(bool block = true);
private:
    rsc::logging::LoggerPtr logger;

    ListenerPtr listener;

    rsc::threading::SynchronizedQueue<EventPtr> queue;

    void handle(EventPtr event);
};

typedef boost::shared_ptr<Reader> ReaderPtr;

}
}
