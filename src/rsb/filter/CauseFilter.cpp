/* ============================================================
 *
 * This file is part of the RSB project.
 *
 * Copyright (C) 2016 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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
 * ============================================================  */

#include "CauseFilter.h"

#include "FilterObserver.h"

using namespace rsc::misc;

namespace rsb {
namespace filter {

CauseFilter::CauseFilter(const UUID& cause,
                         bool        invert):
    causeAsUUID(cause), invert(invert) {
}

CauseFilter::CauseFilter(const EventId& cause,
                         bool           invert):
    causeAsEventId(new EventId(cause)), invert(invert) {
}

UUID CauseFilter::getCause() const {
    if (this->causeAsEventId) {
        return this->causeAsEventId->getAsUUID();
    } else {
        return this->causeAsUUID;
    }
}

bool CauseFilter::isInverted() const {
    return this->invert;
}

bool CauseFilter::match(EventPtr e) {
    bool result = false;
    if (this->causeAsEventId) {
        result = e->isCause(*this->causeAsEventId);
    } else {
        std::set<EventId> causes = e->getCauses();
        for (std::set<EventId>::const_iterator it = causes.begin();
             it != causes.end(); ++it) {
            if (it->getAsUUID() == this->causeAsUUID) {
                result = true;
                break;
            }
        }
    }
    return this->invert ? !result : result;
}

void CauseFilter::notifyObserver(FilterObserverPtr   fo,
                                 FilterAction::Types at) {
    fo->notify(this, at);
}

}
}
