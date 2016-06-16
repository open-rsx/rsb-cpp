/* ============================================================
 *
 * This file is a part of the RSB project
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
 * ============================================================ */

#include "ScopeConverter.h"

#include "rsb/Scope.h"

namespace rsb {
namespace converter {

const std::string ScopeConverter::WIRE_SCHEMA = "scope";

ScopeConverter::ScopeConverter() :
    Converter<std::string> (WIRE_SCHEMA, RSB_TYPE_TAG(Scope)) {
}

ScopeConverter::~ScopeConverter() {
}

std::string ScopeConverter::serialize(const AnnotatedData& data,
                                      std::string&         wire) {
    assert(data.first == this->getDataType());

    wire = boost::static_pointer_cast<Scope>(data.second)->toString();
    return WIRE_SCHEMA;
}

AnnotatedData ScopeConverter::deserialize(const std::string& wireSchema,
                                          const std::string& wire) {
    assert(wireSchema == WIRE_SCHEMA);

    return std::make_pair(getDataType(),
                          boost::shared_ptr<Scope>(new Scope(wire)));
}

}
}
