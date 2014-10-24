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

#include "FilterCombination.h"

namespace rsb {
namespace filter {

FilterCombination::FilterCombination(
	FilterCombination::CombinationStrategy strategy) :
	strategy(strategy)
{
}

bool FilterCombination::match(EventPtr e)
{
	switch (strategy) {
	case MatchAll:
		for(auto it=this->begin(), end=this->end(); it!=end; ++it) {
			if (!(*it)->match(e)) return false;
		}
		return true;
		break;

	case MatchAny:
		for(auto it=this->begin(), end=this->end(); it!=end; ++it) {
			if ((*it)->match(e)) return true;
		}
		return false;
		break;

	default:
		return false; // never reached
	}
}

}
}