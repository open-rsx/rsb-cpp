/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "converters.h"

#include <boost/thread.hpp>

#include "Repository.h"
#include "BoolConverter.h"
#include "StringConverter.h"
#include "Uint64Converter.h"
#include "VoidConverter.h"
#include "ByteArrayConverter.h"

namespace rsb {
namespace converter {

static bool registered = false;
static boost::mutex registrationMutex;

void registerDefaultConverters() {

    boost::mutex::scoped_lock lock(registrationMutex);
    if (!registered) {
        stringConverterRepository()->registerConverter(
                Converter<std::string>::Ptr(new BoolConverter));
        stringConverterRepository()->registerConverter(
                Converter<std::string>::Ptr(new StringConverter));
        stringConverterRepository()->registerConverter(
                Converter<std::string>::Ptr(new Uint64Converter));
        stringConverterRepository()->registerConverter(
                Converter<std::string>::Ptr(new VoidConverter));
        stringConverterRepository()->registerConverter(
                Converter<std::string>::Ptr(new ByteArrayConverter));
        registered = true;
    }

}

}
}
