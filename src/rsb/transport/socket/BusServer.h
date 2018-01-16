/* ============================================================
 *
 * This file is part of the RSB project
 *
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

#include <boost/shared_ptr.hpp>

#include "Bus.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {
namespace socket {

/**
 * Instances of this class provide access to a socket-based bus for
 * local and remote bus clients.
 *
 * Remote clients can connect to a server socket in order to send and
 * receive events through the resulting socket connection.
 *
 * Local clients (connectors) use the usual @ref Bus interface to
 * receive events submitted by remote clients and submit events which
 * will be distributed to remote clients by the @ref BusServer.
 *
 * @author jmoringe
 */
class RSB_EXPORT BusServer : public virtual Bus {
public:
    virtual ~BusServer();

    virtual void activate() = 0;

    virtual void deactivate() = 0;

    virtual void handleIncoming(EventPtr         event,
                                BusConnectionPtr connection) = 0;
};

typedef boost::shared_ptr<BusServer> BusServerPtr;

}
}
}
