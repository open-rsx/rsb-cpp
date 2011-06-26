/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "SimpleImageConverter.h"

#include "SimpleImage.h"

using namespace std;

using namespace boost;

using namespace rsb::converter;

namespace converter_tutorial {

// We have to pass to argument to the base-class constructor:
// 1. The data-type
// 2. The wire-schema
//
// Note: this could also be written as
// Converter<string>("simple-image", RSB_TYPE_TAG(SimpleImage))
// to infer the "string" name of the data-type using RTTI
SimpleImageConverter::SimpleImageConverter() :
    Converter<string> ("converter_tutorial::SimpleImage", "simple-image", true) {
}

string SimpleImageConverter::serialize(const AnnotatedData &data, string &wire) {
    // Ensure that DATA actually holds a datum of the data-type we
    // expect.
    assert(data.first == getDataType()); // this->getDataType() == "converter_tutorial::SimpleImage"

    // Force conversion to the expected data-type.
    // NOTE: a dynamic_pointer_cast cannot be used from void*
    shared_ptr<const SimpleImage> image =
            static_pointer_cast<const SimpleImage> (data.second);

    // Store the content of IMAGE in WIRE according to the selected
    // binary layout.
    int numPixels = image->width * image->height;
    wire.resize(4 + 4 + numPixels);
    copy((char*) &image->width, ((char*) &image->width) + 4, wire.begin());
    copy((char*) &image->height, ((char*) &image->height) + 4, wire.begin() + 4);
    copy((char*) &image->data, ((char*) &image->data) + numPixels,
            wire.begin() + 8);

    // Return the wire-schema of the serialized representation in
    // WIRE.
    return getWireSchema(); // this->getWireSchema() == "simple-image"
}

AnnotatedData SimpleImageConverter::deserialize(const string &wireSchema,
        const string &wire) {
    // Ensure that WIRE uses the expected wire-schema.
    assert(wireSchema == getWireSchema()); // this->getWireSchema() == "simple-image"

    // Allocate a new SimpleImage object and set its data members from
    // the content of WIRE.
    SimpleImage *image = new SimpleImage();
    image->width = *((int*) &*wire.begin());
    image->height = *((int*) &*(wire.begin() + 4));
    image->data = new unsigned char[image->width * image->height];
    copy(wire.begin() + 8, wire.begin() + 8 + image->width * image->height,
            image->data);

    // Return (a shared_ptr to) the constructed object along with its
    // data-type.
    return make_pair(getDataType(), shared_ptr<SimpleImage> (image));
}

}
