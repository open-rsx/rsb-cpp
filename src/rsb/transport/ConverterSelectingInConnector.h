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

#include <rsc/logging/Logger.h>

#include "InConnector.h"
#include "ConverterSelectingConnector.h"

namespace rsb {
namespace transport {

/** This base class for @ref InConnector implementations implements
 * the resolveConverter method of the @ref ConverterSelectingConnector
 * base class by looking up WireSchemas.
 *
 * @author jmoringe
 */
template <typename WireType>
class ConverterSelectingInConnector : public ConverterSelectingConnector<WireType>,
                                      public InConnector {
protected:
    typedef typename ConverterSelectingConnector<WireType>::ConverterPtr ConverterPtr;
    typedef typename ConverterSelectingConnector<WireType>::SignatureType SignatureType;
    typedef typename ConverterSelectingConnector<WireType>::ConverterMap ConverterMap;
    typedef typename ConverterSelectingConnector<WireType>::ConverterNames ConverterNames;

    ConverterSelectingInConnector(const ConverterNames &converters) :
        ConverterSelectingConnector<WireType>("wireSchema", converters),
        logger(rsc::logging::Logger::getLogger("rsb.transport.ConverterSelectingInConnector")) {
    }

    ConverterSelectingInConnector(const ConverterMap &converters) :
        ConverterSelectingConnector<WireType>("wireSchema", converters),
        logger(rsc::logging::Logger::getLogger("rsb.transport.ConverterSelectingInConnector")) {
    }

    /**
     * This function
     * -# looks up the converters designated by @a wireSchema
     * -# disambiguates the candidate set using configuration data, if
     *    necessary
     * -# caches the @ref converter::Converter object in the
     *    connector.
     *
     * @param wireSchema The WireSchema that should be handled by the
     * returned converter.
     * @throw NoSuchObject If no converter can be found for @a wireSchema.
     * @throw runtime_error If multiple converters match and
     * disambiguation is impossible.
     */
    ConverterPtr resolveConverter(const std::string &wireSchema) {
        // Obtain converter candidates for wireSchema.
        std::list<SignatureType> signatures = this->repository->getConverterSignaturesForWireSchema(wireSchema);
        RSCDEBUG(logger, "Converter signatures " << signatures);

        // Try to determine full converter signature.
        SignatureType signature;
        if (signatures.empty()) {
            throw rsc::runtime::NoSuchObject(boost::str(boost::format("No converter for WireSchema `%1%'.")
                                                        % wireSchema));
        } else if (signatures.size() == 1) {
            signature = signatures.front();
        // Ambiguous; Try explicitly configured converters.
        } else if (this->converterNames.find(wireSchema) != this->converterNames.end()) {
            signature = std::make_pair(wireSchema,
                                       this->converterNames.find(wireSchema)->second);
            RSCDEBUG(logger, "Found configured converter signature " << signature);
        // Ambiguous and nothing matching configured.
        } else {
            std::list<std::string> dataTypes;
            for (typename std::list<SignatureType>::const_iterator it = signatures.begin();
                 it != signatures.end(); ++it) {
                dataTypes.push_back(it->second);
            }
            throw std::runtime_error(boost::str(boost::format("Ambiguous converter set for wireType `%1%': %2%; hint: add a configuration option `transport.<name>.converter.%1% = <one of %2%>' to resolve the ambiguity.")
                                                % signatures.front().first % dataTypes));
        }

        RSCDEBUG(logger, "Resolving converter with signature " << signature);
        ConverterPtr converter = this->repository->getConverter(signature);
        RSCDEBUG(logger, "Resolved " << signature << " -> " << converter);
        return converter;
    }
private:
    rsc::logging::LoggerPtr logger;
};

}
}
