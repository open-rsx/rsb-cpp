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

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

#include <rsc/runtime/Printable.h>

#include "Converter.h"

namespace rsb {
namespace converter {

/**
 * Implementation of this interface perform mappings of one of the
 * followings forms:
 * - wire-schema -> @ref Converter
 * - data-type -> @ref Converter
 *
 * @author jmoringe
 */
template <typename WireType>
class ConverterSelectionStrategy: public rsc::runtime::Printable {
public:
    typedef typename Converter<WireType>::Ptr ConverterPtr;

    typedef boost::shared_ptr< ConverterSelectionStrategy<WireType> > Ptr;

    /**
     * Tries to look up the converter designator by @a key.
     *
     * @param key A wire-schema or data-type desinated the desired
     *            converter.
     * @return A boost::shared_ptr holding the converter.
     * @throw rsc::runtime::NoSuchObject If there is no converter fo @a key.
     */
    virtual ConverterPtr getConverter(const std::string& key) const = 0;
};

}
}
