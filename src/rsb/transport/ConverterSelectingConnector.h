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

#include <rsc/logging/Logger.h>

#include "../converter/Converter.h"
#include "../converter/ConverterSelectionStrategy.h"

namespace rsb {
namespace transport {

/**
 * This base class enables look up of suitable
 * @ref rsb::converter::Converter s in
 * @ref rsb::transport::Connector classes.
 *
 * @author jmoringe
 */
template <typename WireType>
class ConverterSelectingConnector {
protected:
    typedef typename converter::Converter<WireType>::Ptr ConverterPtr;
    typedef typename converter::ConverterSelectionStrategy<WireType>::Ptr ConverterSelectionStrategyPtr;

    ConverterSelectingConnector(ConverterSelectionStrategyPtr converters) :
        logger(rsc::logging::Logger::getLogger("rsb.transport.ConverterSelectingConnector")),
        converters(converters) {
    }

    /**
     * Try to find a suitable converter for @a key . It is considered
     * a program error if no such converter can be found. The error
     * condition can be avoided by:
     * -# registering converters for all occuring wire-schemas or data-types
     * -# registering a dummy converter that accepts but discard anything.
     *
     * @param key the wire-schema or data-type of the converter being
     *            requested.
     * @return The requested converter.
     * @throw rsc::runtime::NoSuchObject If no converter could be
     * found for @a key.
     */
    ConverterPtr getConverter(const std::string& key) const {
        return this->converters->getConverter(key);
    }
private:
    rsc::logging::LoggerPtr logger;

    ConverterSelectionStrategyPtr converters;
};

}
}
