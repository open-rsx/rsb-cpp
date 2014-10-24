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

#include "WireSchemaFilter.h"

#include "../MetaData.h"

#include "FilterObserver.h"

namespace rsb {
namespace filter {

WireSchemaFilter::WireSchemaFilter(const std::string& schema,
											  bool               invert):
	 schema(schema), invert(invert) {
}

const std::string& WireSchemaFilter::getSchema() const {
	 return this->schema;
}

bool WireSchemaFilter::isInverted() const {
	 return this->invert;
}

bool WireSchemaFilter::match(EventPtr event) {
     const MetaData& metaData = event->getMetaData();
	 bool result = this->schema == metaData.getUserInfo("rsb.wire-schema");
	 return this->invert ? !result : result;
}

void WireSchemaFilter::notifyObserver(FilterObserverPtr   observer,
												  FilterAction::Types action) {
	 observer->notify(this, action);
}

}
}
