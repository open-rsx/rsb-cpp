/** ============================================================
 *
 * This file is part of the RSB project.
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * ============================================================  */

#pragma once

#include <stdexcept>
#include <string>
#include <iostream>

#include <boost/cstdint.hpp>
#include <boost/format.hpp>

namespace rsb {
namespace transport {
namespace socket {

enum Server {
    SERVER_NO   = 0,
    SERVER_YES  = 1,
    SERVER_AUTO = 2
};

template <typename Ch, typename Tr>
std::basic_istream<Ch, Tr>& operator>>(std::basic_istream<Ch, Tr>& stream, Server& value) {
    // Read one whitespace-delimited token.
    std::basic_string<Ch, Tr> string;
    stream >> string;

    // Interpret the value.
    if (string == "0") {
        value = SERVER_NO;
    } else if (string == "1") {
        value = SERVER_YES;
    } else if (string == "auto") {
        value = SERVER_AUTO;
    } else {
        throw std::invalid_argument(boost::str(boost::format("Invalid server/client specification: %1%")
                                               % string));
    }

    return stream;
}

extern const std::string DEFAULT_HOST;

extern const boost::uint16_t DEFAULT_PORT;

}
}
}
