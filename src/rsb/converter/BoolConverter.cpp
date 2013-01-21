/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2013 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "BoolConverter.h"

#include <sstream>

#include "SerializationException.h"

using namespace std;

namespace rsb {
namespace converter {

const string BoolConverter::WIRE_SCHEMA = "bool";

BoolConverter::BoolConverter() :
    Converter<string> (WIRE_SCHEMA, RSB_TYPE_TAG(bool)) {
}

BoolConverter::~BoolConverter() {
}

string BoolConverter::serialize(const AnnotatedData& data, string& wire) {
    assert(data.first == this->getDataType());

    boost::shared_ptr<bool> s
        = boost::static_pointer_cast<bool>(data.second);
    wire.resize(1);
    wire[0] = ((*s) ? 1 : 0);
    return WIRE_SCHEMA;
}

AnnotatedData BoolConverter::deserialize(const std::string& wireSchema,
                                         const string& wire) {
    assert(wireSchema == WIRE_SCHEMA);

    if (wire.size() == 1 && wire[0] == 0) {
        return make_pair(getDataType(), boost::shared_ptr<bool>(new bool(false)));
    } else if (wire.size() == 1 && wire[0] == 1) {
        return make_pair(getDataType(), boost::shared_ptr<bool>(new bool(true)));
    } else {
        throw runtime_error("Invalid encoding for bool.");
    }
}

}
}
