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

#include "IntrospectionSender.h"

#include <unistd.h> // TODO build abstraction into rsc? does boost have something?

#include <fstream>

#include <boost/format.hpp>

#include <rsb/protocol/operatingsystem/Process.pb.h>

#include "../Factory.h"

#include "Types.h"

using namespace boost;

namespace rsb {
namespace introspection {

IntrospectionSender::IntrospectionSender()
    : logger(rsc::logging::Logger::getLogger("rsb.introspection.IntrospectionSender")),
      listener(getFactory().createListener(INTROSPECTION_PARTICIPANTS_SCOPE)),
      informer(getFactory().createInformerBase(INTROSPECTION_PARTICIPANTS_SCOPE)) {
}

void IntrospectionSender::addParticipant(ParticipantPtr participant) {
    RSCDEBUG(this->logger, boost::format("Adding participant %1%") % participant);

    boost::shared_ptr<rsb::protocol::introspection::Hello> hello(
        new rsb::protocol::introspection::Hello());

    // Add participant information.
    hello->set_id(participant->getId().getId().data,
                  participant->getId().getId().size());
    hello->set_kind("INFORMER");
    hello->set_scope(participant->getScope()->toString());

    // Add host information.
    rsb::protocol::operatingsystem::Host* host
        = hello->mutable_host();
    char buffer[1024];
    gethostname(buffer, 1024); // what could possibly go wrong?
    host->set_id(buffer);
    host->set_hostname(buffer);

    // Add process information.
    rsb::protocol::operatingsystem::Process* process
        = hello->mutable_process();

    process->set_id(boost::lexical_cast<std::string>(getpid()));

    std::ifstream self("/proc/self/cmdline");
    std::string program;
    self >> program;
    process->set_program_name(program);

    this->informer->publish(hello);
}

bool IntrospectionSender::removeParticipant(const Participant& participant) {
    RSCDEBUG(this->logger, boost::format("Removing participant %1%") % participant);

    this->informer->publish(boost::shared_ptr<std::string>(new std::string("bye")));

    return false;
}

}
}
