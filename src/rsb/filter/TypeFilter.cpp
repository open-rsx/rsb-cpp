/* ============================================================
 *
 * This file is part of the RSB project.
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
 * ============================================================  */

#include "TypeFilter.h"

#include "../MetaData.h"

#include "FilterObserver.h"

namespace rsb {
namespace filter {

TypeFilter::TypeFilter(const std::string& type,
                       bool               invert):
    type(type), invert(invert) {
}

const std::string& TypeFilter::getType() const {
    return this->type;
}

bool TypeFilter::isInverted() const {
    return this->invert;
}

bool TypeFilter::match(EventPtr event) {
    bool result = this->type == event->getType();
    return this->invert ? !result : result;
}

void TypeFilter::notifyObserver(FilterObserverPtr   observer,
                                FilterAction::Types action) {
    observer->notify(this, action);
}

}
}
