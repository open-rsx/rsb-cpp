/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 *
 * Written by Ulrich Drepper <drepper@gnu.ai.mit.edu> and heavily modified
 * for GnuPG by <werner.koch@guug.de> and adapted for the need of FPM Blowfish
 * Plugin
 *
 * Latest author:
 * Frederic RUAUDEL <grumz@users.sf.net>
 * FPMBlowfishPlugin
 * Copyleft (c) 2003 Frederic RUAUDEL, all rights reversed
 * Copyleft (C) 1995, 1996, 1998, 1999 Free Software Foundation, Inc.
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

#pragma once

#include <iostream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include "rsb/rsbexports.h"

namespace rsb {
namespace util {

class MD5Hasher;

/**
 * A simple class representing an md5 sum for a given string.
 *
 * @author jwienke
 */
class RSB_EXPORT MD5 {
public:

    /**
     * Creates a new m5s sum for the given string.
     *
     * @param s string to create sum for
     */
    MD5(const std::string& s);

    virtual ~MD5();

    /**
     * Returns a hex encoded string of the sum.
     *
     * @param pretty if @c true, the string will be separated in blocks by
     *               spaces
     */
    std::string toHexString(const bool& pretty = false) const;

private:

    boost::shared_ptr<MD5Hasher> hasher;
    boost::shared_array<unsigned char> hash;

};

RSB_EXPORT std::ostream& operator<<(std::ostream& stream, const MD5& sum);

}
}

