/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include "FilterObserver.h"

#include <iostream>

#include "Filter.h"
#include "ScopeFilter.h"

using namespace std;

namespace rsb {
namespace filter {

FilterObserver::FilterObserver() :
	logger(rsc::logging::Logger::getLogger("rsb.filter.FilterObserver")) {
}

FilterObserver::~FilterObserver() {
}

void FilterObserver::notify(Filter* /*filter*/,
		const FilterAction::Types& /*at*/) {
	RSCDEBUG(logger, "FilterObserver::notify(FilterPtr a)");
}

void FilterObserver::notify(ScopeFilter* /*filter*/,
		const FilterAction::Types& /*at*/) {
	RSCDEBUG(logger, "FilterObserver::notify(ScopeFilterPtr a)");
}

}
}
