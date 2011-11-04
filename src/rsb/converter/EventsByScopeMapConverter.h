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

#include <map>
#include <vector>

#include "../Event.h"
#include "../Scope.h"
#include "Converter.h"
#include "Repository.h"

namespace rsb {
namespace converter {

/**
 * A converter for aggregated events ordered by their scope and time for each
 * scope.
 *
 * @author jwienke
 */
class EventsByScopeMapConverter: public Converter<std::string> {
public:

    /**
     * Constructs a new converter and optionally allows to specify a
     * converter::Repository which will be used for the contained events of
     * arbitrary types.
     *
     * @param serializationConverters converters to use for serialization.
     *                                Defaults to the unambiguous map from
     *                                #stringConverterRepository
     * @param deserializationConverters converters to use for deserialization.
     *                                  Defaults to the unambiguous map from
     *                                  #stringConverterRepository
     */
    EventsByScopeMapConverter(
            ConverterSelectionStrategy<std::string>::Ptr serializationConverters =
                    stringConverterRepository()->getConvertersForSerialization(),
            ConverterSelectionStrategy<std::string>::Ptr deserializationConverters =
                    stringConverterRepository()->getConvertersForDeserialization());
    virtual ~EventsByScopeMapConverter();

    std::string getClassName() const;

    std::string serialize(const AnnotatedData &data, std::string &wire);

    AnnotatedData deserialize(const std::string &wireSchema,
            const std::string &wire);

    std::string getWireSchema() const;

private:
    ConverterSelectionStrategy<std::string>::Ptr serializationConverters;
    ConverterSelectionStrategy<std::string>::Ptr deserializationConverters;
    Converter<std::string>::Ptr converter;

};

}
}

