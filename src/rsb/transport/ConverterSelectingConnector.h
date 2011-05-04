/* ============================================================
 *
 * This file is part of the RSB project.
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
#include <map>

#include <rsc/logging/Logger.h>

#include "../converter/Repository.h"
#include "../converter/Converter.h"

namespace rsb {
namespace transport {

/**
 * @author jmoringe
 */
template <typename WireType>
class ConverterSelectingConnector {
protected:
    typedef typename rsb::converter::Repository<WireType> RepositoryType;
    typedef typename RepositoryType::Ptr RepositoryPtr;
    typedef typename RepositoryType::ConverterSignature SignatureType;
    typedef typename RepositoryType::Converter ConverterPtr;

    /** WireType and DataType for @ref InConnector
     * DataType and WireType for @ref OutConnector
     */
    typedef std::map<std::string, std::string> ConverterNames;

    /** maps WireType -> Converter for @ref InConnector
     * maps DataType -> Converter for @ref OutConnector
     */
    typedef std::map<std::string, ConverterPtr> ConverterMap;

    ConverterSelectingConnector(const std::string &kind,
                                const ConverterMap &converters) :
        logger(rsc::logging::Logger::getLogger("rsb.transport.ConverterSelectingConnector")),
        kind(kind),
        repository(converter::stringConverterRepository()),
        converters(converters) {
    }

    ConverterSelectingConnector(const std::string &kind,
                                const ConverterNames &converterNames) :
        logger(rsc::logging::Logger::getLogger("rsb.transport.ConverterSelectingConnector")),
        kind(kind),
        repository(converter::stringConverterRepository()),
        converterNames(converterNames) {
    }

    virtual ConverterPtr resolveConverter(const std::string &key) = 0;

    /** Try to find a suitable converter for @a key by
     * -# looking in the cache of known converters
     * -# call @ref resolveConverter if necessary
     *
     * @param key the WireSchema or DataType of the converter being
     *            requested.
     * @return The requested converter.
     * @throw rsc::runtime::NoSuchObject If no could be found for @a key.
     * @throw srd::runtime_error If multiple converters where found
     * for @a key.
     *
     * @see resolveConverter
     */
    ConverterPtr getConverter(const std::string &key) {
        // Try to find the converter in our cache.
        typename ConverterMap::const_iterator it = this->converters.find(key);

        // Cache hit; we are done.
        if (it != this->converters.end())
            return it->second;

        // Cache miss; Ask repository.
        RSCDEBUG(logger, "No cache entry for " << this->kind
                 << " `" << key << "'; initiating lookup");
        ConverterPtr converter = resolveConverter(key);
        this->converters[key] = converter;
        return converter;
    }
private:
    rsc::logging::LoggerPtr logger;
    std::string kind;
protected:
    RepositoryPtr repository;
    ConverterMap converters;
    ConverterNames converterNames;
};

}
}
