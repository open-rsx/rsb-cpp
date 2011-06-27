/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <rsb/Factory.h>
#include <rsb/converter/Repository.h>
#include <rsb/converter/ProtocolBufferConverter.h>

// See ../CMakeLists.txt for the generation of this file.
// The generated file can be found in ${BUILD_DIR}/protobuf_converter
#include <protobuf_converter/SimpleImage.pb.h>

using namespace boost;

using namespace rsb;
using namespace rsb::converter;

// The generated protocol buffer class is in this namespace.
using namespace tutorial::protobuf_converter;

int main() {
    // Register a specific template instanciation of the
    // ProtocolBufferConverter for our SimpleImage protocol buffer
    // message.
    shared_ptr<ProtocolBufferConverter<SimpleImage> > converter(
            new ProtocolBufferConverter<SimpleImage> ());
    stringConverterRepository()->registerConverter(converter);

    // Create an informer which has the SimpleImage protocol buffer
    // message as its data type.
    Informer<SimpleImage>::Ptr informer =
            Factory::getInstance().createInformer<SimpleImage> (
                    Scope("/tutorial/converter"));

    // Create and publish an instance of SimpleImage. To see the
    // event, you can, for example use the RSB logger utility or the
    // receiver program in this directory.
    Informer<SimpleImage>::DataPtr data(new SimpleImage());
    data->set_width(10);
    data->set_height(10);
    data->set_data(new char[100], 100);
    informer->publish(data);

    return EXIT_SUCCESS;
}
