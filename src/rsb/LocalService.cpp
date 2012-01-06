/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "LocalService.h"

#include <sstream>

using namespace std;

namespace rsb {

LocalService::LocalService(const Scope& scope) :
    Service(scope) {
}

LocalService::~LocalService() {
}

string LocalService::getClassName() const {
    return "LocalService";
}

set<ParticipantPtr> LocalService::getParticipants() const {
    boost::recursive_mutex::scoped_lock lock(mutex);
    return participants;
}

void LocalService::addParticipant(ParticipantPtr participant) {
    if (!participant->getScope()->isSubScopeOf(*getScope())) {
        stringstream s;
        s << "Cannot add Participant " << *participant << " with scope "
                << participant->getScope() << " to this service with scope "
                << *getScope()
                << " because the participant is not in a sub-scope.";
        throw invalid_argument(s.str());
    }
    boost::recursive_mutex::scoped_lock lock(mutex);
    participants.insert(participant);
}

void LocalService::removeParticipant(ParticipantPtr participant) {
    boost::recursive_mutex::scoped_lock lock(mutex);
    participants.erase(participant);
}

}
