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

#include "SimpleImageConverter.h"

#include "SimpleImage.h"

using namespace std;

using namespace boost;

using namespace rsb;
using namespace rsb::converter;

namespace converter_tutorial {

// We have to pass two arguments to the base-class constructor:
// 1. The data-type
// 2. The wire-schema
//
// Note: this could also be written as
// Converter<string>("simple-image", RSB_TYPE_TAG(SimpleImage))
// to infer the "string" name of the data-type using RTTI.
SimpleImageConverter::SimpleImageConverter() :
    Converter<string> ("converter_tutorial::SimpleImage", "simple-image", true) {
}

string SimpleImageConverter::serialize(const AnnotatedData& data, string& wire) {
    // Ensure that DATA actually holds a datum of the data-type we
    // expect.
    assert(data.first == getDataType()); // this->getDataType() == "converter_tutorial::SimpleImage"

    // Force conversion to the expected data-type.
    //
    // NOTE: a dynamic_pointer_cast cannot be used from void*
    boost::shared_ptr<const SimpleImage> image =
            static_pointer_cast<const SimpleImage> (data.second);

    // Store the content of IMAGE in WIRE according to the selected
    // binary layout.
    //
    // NOTE: do not use this kind of "serialization" for any real code.
    int numPixels = image->width * image->height;
    wire.resize(4 + 4 + numPixels);
    copy((char*) &image->width, ((char*) &image->width) + 4, wire.begin());
    copy((char*) &image->height, ((char*) &image->height) + 4, wire.begin() + 4);
    copy((char*) image->data, ((char*) image->data) + numPixels,
            wire.begin() + 8);

    // Return the wire-schema of the serialized representation in
    // WIRE.
    return getWireSchema(); // this->getWireSchema() == "simple-image"
}

AnnotatedData SimpleImageConverter::deserialize(const string& wireSchema,
        const string& wire) {
    // Ensure that WIRE uses the expected wire-schema.
    assert(wireSchema == getWireSchema()); // this->getWireSchema() == "simple-image"

    // Allocate a new SimpleImage object and set its data members from
    // the content of WIRE.
    //
    // NOTE: do not use this kind of "deserialization" for any real
    // code.
    SimpleImage* image = new SimpleImage();
    image->width = *((int*) &*wire.begin());
    image->height = *((int*) &*(wire.begin() + 4));
    image->data = new unsigned char[image->width * image->height];
    copy(wire.begin() + 8, wire.begin() + 8 + image->width * image->height,
            image->data);

    // Return (a shared_ptr to) the constructed object along with its
    // data-type.
    return make_pair(getDataType(), boost::shared_ptr<SimpleImage> (image));
}

}
