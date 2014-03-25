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

#include "MethodFilter.h"

#include "../MetaData.h"

#include "FilterObserver.h"

namespace rsb {
namespace filter {

MethodFilter::MethodFilter(const std::string& method,
                           bool               invert):
    method(method), invert(invert) {
}

const std::string& MethodFilter::getMethod() const {
    return this->method;
}

bool MethodFilter::isInverted() const {
    return this->invert;
}

bool MethodFilter::match(EventPtr event) {
    bool result = this->method == event->getMethod();
    return this->invert ? !result : result;
}

void MethodFilter::notifyObserver(FilterObserverPtr   observer,
                                  FilterAction::Types action) {
    observer->notify(this, action);
}

}
}
