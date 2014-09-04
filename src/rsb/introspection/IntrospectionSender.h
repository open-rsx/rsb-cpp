/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2014 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>

#include <boost/thread/mutex.hpp>

#include <rsc/logging/Logger.h>

#include "../Participant.h"
#include "../Listener.h"
#include "../Informer.h"

#include "../patterns/LocalServer.h"

#include "Model.h"

namespace rsb {
namespace introspection {

/**
 * Instances of this class publish information about the local host,
 * the current process and its participants for consumption by
 * introspection receivers.
 *
 * @author jmoringe
 */
class IntrospectionSender {
friend struct QueryHandler;
public:
    IntrospectionSender();

    void addParticipant(ParticipantPtr participant,
                        ParticipantPtr parent);

    bool removeParticipant(const Participant& participant);
private:
    typedef std::vector<ParticipantInfo> ParticipantList;

    rsc::logging::LoggerPtr       logger;

    boost::mutex                  mutex;

    ParticipantList               participants;
    ProcessInfo                   process;
    HostInfo                      host;

    ListenerPtr                   listener;
    InformerBasePtr               informer;

    rsb::patterns::LocalServerPtr server;

    void sendHello(const ParticipantInfo& participant,
                   EventPtr               query = EventPtr());

    void sendBye(const ParticipantInfo& participant);
};

typedef boost::shared_ptr<IntrospectionSender> IntrospectionSenderPtr;

}
}
