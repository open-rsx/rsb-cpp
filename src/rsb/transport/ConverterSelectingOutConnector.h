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

#include "OutConnector.h"
#include "ConverterSelectingConnector.h"

namespace rsb {
namespace transport {

/** This base class for @ref OutConnector implementations implements
 * the resolveConverter method of the @ref ConverterSelectingConnector
 * base class by looking up DataTypes.
 *
 * @author jmoringe
 */
template <typename WireType>
class ConverterSelectingOutConnector : public ConverterSelectingConnector<WireType>,
                                       public OutConnector {
protected:
    typedef typename ConverterSelectingConnector<WireType>::ConverterPtr ConverterPtr;
    typedef typename ConverterSelectingConnector<WireType>::SignatureType SignatureType;
    typedef typename ConverterSelectingConnector<WireType>::ConverterMap ConverterMap;
    typedef typename ConverterSelectingConnector<WireType>::ConverterNames ConverterNames;

    ConverterSelectingOutConnector(const ConverterNames &converters) :
        ConverterSelectingConnector<WireType>("dataType", converters),
        logger(rsc::logging::Logger::getLogger("rsb.transport.ConverterSelectingOutConnector")) {
    }

    ConverterSelectingOutConnector(const ConverterMap &converters) :
        ConverterSelectingConnector<WireType>("dataType", converters),
        logger(rsc::logging::Logger::getLogger("rsb.transport.ConverterSelectingOutConnector")) {
    }

    /**
     * This function
     * -# looks up the converters designated by @a dataType
     * -# disambiguates the candidate set using configuration data, if
     *    necessary
     * -# caches the @ref converter::Converter object in the
     *    connector.
     *
     * @param dataType The DataType that should be handled by the
     * returned converter.
     * @throw rsc::runtime::NoSuchObject If no converter can be found
     * for @a dataType.
     * @throw std::runtime_error If multiple converters match and
     * disambiguation is impossible.
     */
    ConverterPtr resolveConverter(const std::string &dataType) {
        // Obtain converter candidates for dataType.
        std::list<SignatureType> signatures = this->repository->getConverterSignaturesForDataType(dataType);
        RSCDEBUG(this->logger, "Converter signatures " << signatures);

        // Try to determine full converter signature.
        SignatureType signature;
        if (signatures.empty()) {
            throw rsc::runtime::NoSuchObject(boost::str(boost::format("No converter for DataType `%1%'.")
                                                        % dataType));
        // Exactly one signature; just use it.
        } else if (signatures.size() == 1) {
            signature = signatures.front();
        // Ambiguous; Try explicitly configured converters.
        } else if (this->converterNames.find(dataType) != this->converterNames.end()) {
            signature = std::make_pair(this->converterNames.find(dataType)->second, dataType);
            RSCDEBUG(this->logger, "Found configured converter signature " << signature);
        // Ambiguous and nothing matching configured.
        } else {
            std::list<std::string> wireTypes;
            for (typename std::list<SignatureType>::const_iterator it = signatures.begin();
                 it != signatures.end(); ++it) {
                wireTypes.push_back(it->first);
            }
            throw std::runtime_error(boost::str(boost::format("Ambiguous converter set for dataType `%2%': %1%; hint: add a configuration option `transport.<name>.converter.<one of %1%> = %2%' to resolve the ambiguity.")
                                                % wireTypes % signatures.front().second));
        }

        RSCDEBUG(this->logger, "Resolving converter with signature " << signature);
        ConverterPtr converter = this->repository->getConverter(signature);
        RSCDEBUG(this->logger, "Resolved " << signature << " -> " << converter);
        return converter;
    }
private:
    rsc::logging::LoggerPtr logger;
};

}
}
