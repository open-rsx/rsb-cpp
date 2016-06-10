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

#pragma once

#include <rsc/misc/UUID.h>

#include <rsb/EventId.h>

#include "Filter.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace filter {

/**
 * This filter matches events in which a particular cause is present.
 *
 * @author jmoringe
 */
class RSB_EXPORT CauseFilter: public Filter {
public:
    /**
     * Creates a new cause filter that matches event in whose cause
     * vector @a cause is present.
     *
     * @param cause The cause that has to be present in events
     *              matching the filter.
     * @param invert If true, events match if @a cause is @b not
     *               present in their cause vector.
     */
    CauseFilter(const rsc::misc::UUID& cause,
                bool                   invert = false);

    /**
     * Creates a new cause filter that matches event in whose cause
     * vector @a cause is present.
     *
     * @param cause The cause that has to be present in events
     *              matching the filter.
     * @param invert If true, events match if @a cause is @b not
     *               present in their cause vector.
     */
    CauseFilter(const EventId& cause,
                bool           invert = false);

    /**
     * Return the cause that has to be present in events matched by
     * this filter.
     *
     * @return The cause potentially converted to a @ref
     *         rsc::misc::UUID.
     */
    rsc::misc::UUID getCause() const;

    /**
     * Indicates whether the filter is inverted.
     *
     * @return @c true if the filter is inverted.
     */
    bool isInverted() const;

    bool match(EventPtr e);

    void notifyObserver(FilterObserverPtr fo, FilterAction::Types at);
private:
    EventIdPtr      causeAsEventId;
    rsc::misc::UUID causeAsUUID;
    bool            invert;
};

}
}
