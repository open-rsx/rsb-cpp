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

#include "Participant.h"

#include "Scope.h"
#include "ParticipantConfig.h"

using namespace std;

namespace rsb {

class Participant::Impl {
public:
    rsc::misc::UUID id;
    ScopePtr scope;
    ParticipantConfig config;
};

Participant::Participant(const Scope& scope, const ParticipantConfig& config) :
    d(new Impl) {
    d->scope.reset(new Scope(scope));
    d->config = config;
}

Participant::~Participant() {
}

rsc::misc::UUID Participant::getId() const {
    return d->id;
}

ScopePtr Participant::getScope() const {
    return d->scope;
}

ParticipantConfig Participant::getConfig() const {
    return d->config;
}

void Participant::printContents(ostream& stream) const {
    stream << "id = " << d->id << ", scope = " << *d->scope;
}

}
