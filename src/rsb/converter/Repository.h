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

#pragma once

#include <string>
#include <stdexcept>

#include <boost/format.hpp>

#include <rsc/runtime/NoSuchObject.h>

#include "Converter.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace converter {

/**
 * Maintains a collection of converters for a specific wire format. Each
 * converter has a wire type describing the actual message that is written on
 * the wire and a data type that indicates which data it can serialize on the
 * wire.
 *
 * @author jwienke
 * @tparam WireType the wire-type of the collected converters.
 */
template<class WireType>
class Repository {
public:

    typedef typename Converter<WireType>::Ptr Converter;

    /** WireSchema and DataType */
    typedef std::pair<std::string, std::string> ConverterSignature;

    std::list<ConverterSignature> getConverterSignatures() const {
        std::list<ConverterSignature> result;
        for (typename ConverterMap::const_iterator it = this->converters.begin();
             it != this->converters.end(); ++it) {
            result.push_back(it->first);
        }
        return result;
    }

    std::list<ConverterSignature> getConverterSignaturesForWireSchema(const std::string &wireSchema) const {
        std::list<ConverterSignature> result;
        for (typename ConverterMap::const_iterator it = this->converters.begin();
             it != this->converters.end(); ++it) {
            if (it->first.first == wireSchema) {
                result.push_back(it->first);
            }
        }
        return result;
    }

    std::list<ConverterSignature> getConverterSignaturesForDataType(const std::string &dataType) const {
        std::list<ConverterSignature> result;
        for (typename ConverterMap::const_iterator it = this->converters.begin();
             it != this->converters.end(); ++it) {
            if (it->first.second == dataType) {
                result.push_back(it->first);
            }
        }
        return result;
    }

    /**
     * Registers the given converter in the collection.
     *
     * @param converter the converter to register
     * @throw std::invalid_argument if there is already a converter registered
     *                              with the same wire type or data type
     */
    void registerConverter(Converter converter) {
        std::string wireSchema = converter->getWireSchema();
        std::string dataType   = converter->getDataType();
        typename ConverterMap::const_iterator it
            = this->converters.find(std::make_pair(wireSchema, dataType));
        if (it != this->converters.end()) {
            throw std::invalid_argument(boost::str(boost::format("There already is a converter for wire-schema `%1%' and data-type `%2%'")
                                                   % wireSchema % dataType));
        }
        this->converters[std::make_pair(wireSchema, dataType)]
            = converter;
    }

    Converter getConverter(const std::string &wireSchema, const std::string &dataType) const {
        std::cout << this->converters << std::endl;
        typename ConverterMap::const_iterator it
            = this->converters.find(std::make_pair(wireSchema, dataType));
        if (it == this->converters.end()) {
            throw rsc::runtime::NoSuchObject(boost::str(boost::format("Could not find a converter for wire-schema `%1%' and data-type `%2%'")
                                                        % wireSchema % dataType));
        }
        return it->second;
    }

    Converter getConverter(const ConverterSignature &signature) const {
        return getConverter(signature.first, signature.second);
    }


    typedef boost::shared_ptr<Repository<WireType> > Ptr;

private:
    typedef std::map<ConverterSignature, Converter> ConverterMap;

    ConverterMap converters;
};

RSB_EXPORT Repository<std::string>::Ptr stringConverterRepository();

}
}
