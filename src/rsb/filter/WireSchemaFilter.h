/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2014 by Robert Haschke <rhaschke at techfak dot uni-bielefeld dot de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the "LGPL"),
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
 *   CITEC, "Cognitive Interaction Technology" Excellence Cluster
 *     Bielefeld University
 *
 * ============================================================ */

#pragma once

#include "Filter.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace filter {

/**
 * This filter matches events based on the type of their wire schema.
 * In contract to @ref TypeFilter the WireSchemaFilter can be applied
 * before payload conversion.
 *
 * @author rhaschke
 */
class RSB_EXPORT WireSchemaFilter: public Filter {
public:
	 /**
	  * Creates a new wire schema filter that matches events whose
	  * wire schema is @a schema.
	  *
	  * @param schema String value of the desired wire schema.
	  * @param invert If true, events match if their wire schema fields do
	  *               @b not match @a schema.
	  */
	 WireSchemaFilter(const std::string& schema,
							bool               invert = false);

	 const std::string& getSchema() const;

	 bool isInverted() const;

	 bool match(EventPtr event);

	 void notifyObserver(FilterObserverPtr observer,
								FilterAction::Types action);
private:
	 std::string schema;
	 bool        invert;
};

}
}
