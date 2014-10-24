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
#include <rsb/filter/Filter.h>
#include <vector>

namespace rsb {
namespace filter {

/** A filter composed of individual sub filters.
 *
 *  Two different combination strategies are possible:
 *  MatchAll: all sub filters must match for a final match (AND combination)
 *  MatchAny: any sub filter may match for a final match (OR combination)
 */
class FilterCombination :
        public rsb::filter::Filter,
        public std::vector<rsb::filter::FilterPtr>
{
public:
    enum CombinationStrategy {MatchAll, MatchAny};
    FilterCombination(CombinationStrategy strategy = MatchAll);

    void setStrategy(CombinationStrategy strategy) {this->strategy = strategy;}
    CombinationStrategy getStrategy() const {return this->strategy;}

    bool match(EventPtr e);

private:
    CombinationStrategy strategy;
};

typedef boost::shared_ptr<FilterCombination> FilterCombinationPtr;

}
}
