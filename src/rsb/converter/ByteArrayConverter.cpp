/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2012 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "ByteArrayConverter.h"

#include <rsc/misc/Registry.h>

using namespace std;

namespace rsb {
namespace converter {

const string ByteArrayConverter::WIRE_SCHEMA = ".*";

ByteArrayConverter::ByteArrayConverter() :
    Converter<string> ("bytearray", WIRE_SCHEMA, true) {
}

ByteArrayConverter::~ByteArrayConverter() {
}

string ByteArrayConverter::serialize(const AnnotatedData& /*data*/, string& /*wire*/) {
    throw runtime_error("This converter should only be used for *de*serialization.");
}

AnnotatedData ByteArrayConverter::deserialize(const std::string& /*wireType*/,
                                              const string& wire) {
    return make_pair(getDataType(), boost::shared_ptr<string>(new string(wire.c_str(), wire.length())));
}

}
}
