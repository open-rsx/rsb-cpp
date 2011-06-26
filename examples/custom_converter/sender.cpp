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

#include <boost/shared_ptr.hpp>

#include <rsb/Factory.h>
#include <rsb/converter/Repository.h>

#include "SimpleImage.h"
#include "SimpleImageConverter.h"

using namespace boost;

using namespace rsb;
using namespace rsb::converter;

using namespace converter_tutorial;

int main() {
    // Register our converter within the collection of converters for
    // the string wire-type (which is used for arrays of octets in
    // C++).
    //
    // Try senderNoConverter.cpp to see what happens, if the converter
    // is not registered.
    shared_ptr<SimpleImageConverter> converter(new SimpleImageConverter());
    stringConverterRepository()->registerConverter(converter);

    // Create an Informer object that is parametrized with the
    // data-type SimpleImage.
    Informer<SimpleImage>::Ptr informer =
            Factory::getInstance().createInformer<SimpleImage> (
                    Scope("/tutorial/converter"));

    // Construct an send a SimpleImage object.
    shared_ptr<SimpleImage> image(new SimpleImage());
    image->width = 10;
    image->height = 10;
    image->data = new unsigned char[100];
    informer->publish(image);

    return EXIT_SUCCESS;
}
