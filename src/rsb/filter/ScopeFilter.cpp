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

#include "ScopeFilter.h"
#include "FilterObserver.h"

using namespace std;

namespace rsb {
namespace filter {

ScopeFilter::ScopeFilter(const Scope& scope) :
    scope(scope) {
}

ScopeFilter::~ScopeFilter() {
}

string ScopeFilter::getClassName() const {
    return "ScopeFilter";
}

void ScopeFilter::printContents(ostream& stream) const {
    stream << "scope = " << scope;
}

bool ScopeFilter::match(EventPtr e) {
    if ((*e->getScopePtr() == this->scope) || e->getScopePtr()->isSubScopeOf(
            this->scope)) {
        return true;
    } else {
        return false;
    }
}

void ScopeFilter::notifyObserver(FilterObserverPtr fo, FilterAction::Types at) {
    fo->notify(this, at);
}

Scope ScopeFilter::getScope() {
    return scope;
}

}
}
