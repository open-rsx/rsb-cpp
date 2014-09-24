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

#include <rsc/runtime/TypeStringTools.h>

#include "Filter.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace filter {

/**
 * This filter matches events based on the data type of their payload.
 *
 * @author jmoringe
 */
class RSB_EXPORT TypeFilter: public Filter {
public:
    /**
     * Creates a new type filter that matches events whose payload is
     * of type @c T.
     *
     * @tparam T The type matching event payloads must have.
     * @param invert If true, events match if their type fields do
     *               @b not match @a type.
     * @return A pointer to the newly created object.
     */
    template <typename T>
    static TypeFilter* createForType(bool invert = false) {
        return new TypeFilter(rsc::runtime::typeName<T>(), invert);
    }

    /**
     * Creates a new type filter that matches events whose payload is
     * of the type designated by @a type.
     *
     * @param type String designating the which the payload of
     *             matching events has to have.
     * @param invert If true, events match if their payload type does
     *               @b not match @a type.
     */
    TypeFilter(const std::string& type,
               bool               invert = false);

    const std::string& getType() const;

    bool isInverted() const;

    bool match(EventPtr event);

    void notifyObserver(FilterObserverPtr observer,
                        FilterAction::Types action);
private:
    std::string type;
    bool        invert;
};

}
}
