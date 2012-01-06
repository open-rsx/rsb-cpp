/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "SchemaAndByteArrayConverter.h"

#include <utility>

using namespace std;

namespace rsb {
namespace converter {

// TODO what to do with the wire schema? This will always change...
SchemaAndByteArrayConverter::SchemaAndByteArrayConverter() :
        Converter<string>("schemaandbytearray", "any", true) {
}

SchemaAndByteArrayConverter::~SchemaAndByteArrayConverter() {
}

string SchemaAndByteArrayConverter::serialize(const AnnotatedData& data,
        string& wire) {

    boost::shared_ptr<pair<string, boost::shared_ptr<string> > > realData =
            boost::static_pointer_cast<pair<string, boost::shared_ptr<string> > >(
                    data.second);

    wire = *(realData->second);
    return realData->first;

}

AnnotatedData SchemaAndByteArrayConverter::deserialize(const string& wireSchema,
        const string& wire) {
    return make_pair(
            getDataType(),
            boost::shared_ptr<pair<string, boost::shared_ptr<void> > >(
                    new pair<string, boost::shared_ptr<void> >(wireSchema,
                            boost::shared_ptr<string>(new string(wire)))));
}

}
}
