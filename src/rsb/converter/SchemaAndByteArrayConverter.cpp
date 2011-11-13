/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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
