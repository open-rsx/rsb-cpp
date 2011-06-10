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

#include <stdexcept>
#include <map>
#include <string>

#include <boost/format.hpp>

#include <rsc/runtime/NoSuchObject.h>
#include <rsc/runtime/Printable.h>

#include "ConverterSelectionStrategy.h"

namespace rsb {
namespace converter {

/**
 * Objects this class store mappings of one of the followings forms
 * - wire-schema -> @ref Converter
 * - data-type -> @ref Converter
 * This association is unambiguous.
 *
 * @author jmoringe
 */
template <typename WireType>
class UnambiguousConverterMap: public ConverterSelectionStrategy<WireType> {
public:
    typedef typename ConverterSelectionStrategy<WireType>::ConverterPtr ConverterPtr;

    /**
     * Tries to look up the converter designator by @a key.
     *
     * @param key A wire-schema or data-type desinated the desired
     *            converter.
     * @return A boost::shared_ptr holding the converter.
     * @throw rsc::runtime::NoSuchObject If there is no converter fo @a key.
     */
    ConverterPtr getConverter(const std::string &key) const {
        typename ConverterMap::const_iterator it
            = this->converters.find(key);
        if (it == this->converters.end()) {
            throw rsc::runtime::NoSuchObject(key);
        }
        return it->second;
    }

    /**
     * Stores @a converter in the map under the name @a key.
     *
     * @param key Either a wire-schema or a data-type depending on the
     * context in which the map is used.
     * @param converter The converter that should be added.
     * @throw std::invalid_argument If there already is a converter
     * for the name @a key.
     * @note @ref Converter::getDataType and @ref
     * Converter::getWireSchema are not considered in any way.
     */
    void addConverter(const std::string &key, ConverterPtr converter) {
        // TODO use RSB exception class, but do we have one for invalid argument?
        if (this->converters.find(key) != this->converters.end())
            throw std::invalid_argument(boost::str(boost::format("A converter is already stored for the key `%1%'")
                                                                 % key));
        this->converters.insert(std::make_pair(key, converter));
    }
private:
    typedef std::map<std::string, ConverterPtr> ConverterMap;

    ConverterMap converters;

    std::string getClassName() const {
        return "UnambiguousConverterMap";
    }

    void printContents(std::ostream &stream) const {
        stream << "converters = " << this->converters;
    }
};

}
}
