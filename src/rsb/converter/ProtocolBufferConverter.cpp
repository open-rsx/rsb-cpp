/* ============================================================
 *
 * This file is a part of the RSB-trunk project.
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

#include "ProtocolBufferConverter.h"

namespace rsb {
namespace converter {

namespace detail {

std::string typeNameToProtoName(const std::string& type_name) {
    std::string result = ".";
    bool colon = false;
    for (std::string::const_iterator it = type_name.begin(); it
             != type_name.end(); ++it) {
        // Consume two (hopefully adjacent) ':', emit one '.'
        if (*it == ':') {
            if (colon) {
                colon = false;
            } else {
                result.push_back('.');
                colon = true;
            }
        } else {
            result.push_back(*it);
        }
    }
    return result;
}

std::string typeNameToWireSchema(const std::string& type_name) {
    // See https://code.cor-lab.de/projects/rsb/wiki/Types/diff?version=6&version_from=4&commit=View+differences
    // return "protocol-buffer:" + typeNameToProtoName(type_name);
    return typeNameToProtoName(type_name);
}

}

}
}
