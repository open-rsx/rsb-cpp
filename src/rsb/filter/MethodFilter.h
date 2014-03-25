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

#pragma once

#include "Filter.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace filter {

/**
 * This filter matches events based on the value of their method
 * field.
 *
 * @author jmoringe
 */
class RSB_EXPORT MethodFilter: public Filter {
public:
    /**
     * Creates a new method filter that matches events whose method
     * fields have the value @a method.
     *
     * @param method String value that the method field of matched
     *               events has to have.
     * @param invert If true, events match if their method fields do
     *               @b not match @a method.
     */
    MethodFilter(const std::string& method,
                 bool               invert = false);

    const std::string& getMethod() const;

    bool isInverted() const;

    bool match(EventPtr event);

    void notifyObserver(FilterObserverPtr observer,
                        FilterAction::Types action);
private:
    std::string method;
    bool        invert;
};

}
}
