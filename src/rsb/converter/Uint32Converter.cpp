/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2013 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "Uint32Converter.h"

#include <sstream>

#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>

#include "SerializationException.h"

using namespace std;

using namespace boost;

namespace rsb {
namespace converter {

const string Uint32Converter::WIRE_SCHEMA = "uint32";

Uint32Converter::Uint32Converter() :
	Converter<string> (WIRE_SCHEMA, RSB_TYPE_TAG(uint32_t)) {
}

Uint32Converter::~Uint32Converter() {
}

string Uint32Converter::serialize(const AnnotatedData& data, string& wire) {
    assert(data.first == this->getDataType());

    boost::shared_ptr<uint32_t> number = boost::static_pointer_cast<uint32_t>(data.second);
    wire.resize(4);
    for (uint32_t i = 0; i < 4; ++i) {
        wire[i] = (unsigned char) ((*number & (0xffull << (i * 8ull))) >> (i * 8ull));
    }
    return WIRE_SCHEMA;
}

AnnotatedData Uint32Converter::deserialize(const string& wireSchema,
                                           const string& wire) {
    assert(wireSchema == WIRE_SCHEMA);
    assert(wire.size() == 4);

    boost::shared_ptr<uint32_t> number(new uint32_t(0));
    for (uint32_t i = 0; i < 4; ++i) {
        *number |= ((uint32_t) ((unsigned char) wire[i]) << (i * 8ull));
    }
    return make_pair(getDataType(), number);
}

}
}
