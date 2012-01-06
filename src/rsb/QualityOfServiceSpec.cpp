/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "QualityOfServiceSpec.h"

using namespace std;

namespace rsb {

QualityOfServiceSpec::QualityOfServiceSpec() :
    ordering(UNORDERED), reliability(RELIABLE) {
}

QualityOfServiceSpec::QualityOfServiceSpec(Ordering ordering,
        Reliability reliability) :
    ordering(ordering), reliability(reliability) {
}

QualityOfServiceSpec::~QualityOfServiceSpec() {
}

QualityOfServiceSpec::Ordering QualityOfServiceSpec::getOrdering() const {
    return ordering;
}

QualityOfServiceSpec::Reliability QualityOfServiceSpec::getReliability() const {
    return reliability;
}

bool QualityOfServiceSpec::operator==(const QualityOfServiceSpec& other) const {
    return (ordering == other.ordering && reliability == other.reliability);
}

ostream& operator<<(ostream& stream, const QualityOfServiceSpec& spec) {
    return stream << "QualityOfServiceSpec[ordering = " << spec.ordering
            << ", reliability = " << spec.reliability << "]";
}

}
