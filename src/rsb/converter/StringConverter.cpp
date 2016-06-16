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

#include "StringConverter.h"

using namespace std;

namespace rsb {
namespace converter {

const string StringConverter::WIRE_SCHEMA = "utf-8-string";

StringConverter::StringConverter() :
    Converter<string> (WIRE_SCHEMA, RSB_TYPE_TAG(string)) {
}

StringConverter::~StringConverter() {
}

string StringConverter::serialize(const AnnotatedData& data,
                                  string&              wire) {
    assert(data.first == this->getDataType());

    // Essentially return the contained string to the serialization
    // medium.
    wire = *boost::static_pointer_cast<string>(data.second);
    return WIRE_SCHEMA;
}

AnnotatedData StringConverter::deserialize(const std::string& wireSchema,
                                           const string&      wire) {
    assert(wireSchema == WIRE_SCHEMA);

    return make_pair(getDataType(), boost::shared_ptr<string>(new string(wire)));
}

}
}
