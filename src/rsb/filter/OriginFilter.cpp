/* ============================================================
 *
 * This file is part of the RSB project.
 *
 * Copyright (C) 2011, 2014 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "OriginFilter.h"

#include "../EventId.h"
#include "../MetaData.h"

#include "FilterObserver.h"

using namespace rsc::misc;

namespace rsb {
namespace filter {

OriginFilter::OriginFilter(const UUID& origin,
                           bool        invert):
    origin(origin), invert(invert) {
}

UUID OriginFilter::getOrigin() const {
    return this->origin;
}

bool OriginFilter::isInverted() const {
    return this->invert;
}

bool OriginFilter::match(EventPtr e) {
    bool result = this->origin == e->getId().getParticipantId();
    return this->invert ? !result : result;
}

void OriginFilter::notifyObserver(FilterObserverPtr   fo,
                                  FilterAction::Types at) {
    fo->notify(this, at);
}

}
}
