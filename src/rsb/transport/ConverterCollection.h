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

#include "Converter.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {

/**
 * Maintains a collection of converters for a specific wire format. Each
 * converter has a wire type describing the actual message that is written on
 * the wire and a data type that indicates which data it can serialize on the
 * wire.
 *
 * @author jwienke
 * @tparam WireFormat the wire format of the collected converters
 */
template<class WireFormat>
class ConverterCollection {
public:

	typedef typename Converter<WireFormat>::Ptr Converter;

	/**
	 * Registers the given converter in the collection.
	 *
	 * @param converter the converter to register
	 * @throw std::invalid_argument if there is already a converter registered
	 *                              with the same wire type or data type
	 */
	void registerConverter(Converter converter) {

		if (wireSchemaMap.count(converter->getWireSchema())) {
			throw std::invalid_argument("Wire schema '"
					+ converter->getWireSchema()
					+ "' already exists in the collection.");
		}
		if (dataTypeMap.count(converter->getDataType())) {
			throw std::invalid_argument("Data type '"
					+ converter->getDataType()
					+ "' already exists in the collection.");
		}

		wireSchemaMap[converter->getWireSchema()] = converter;
		dataTypeMap[converter->getDataType()] = converter;

	}

	/**
	 * Accesses a the converter for the specified wire schema.
	 *
	 * @param wireSchema wire schema to find a converter for
	 * @return the converter
	 * @throw std::invalid_argument no converter for the given schema
	 */
	Converter getConverterByWireSchema(const std::string &wireSchema) const {

		typename std::map<std::string, Converter>::const_iterator it =
				wireSchemaMap.find(wireSchema);
		if (it == wireSchemaMap.end()) {
			throw std::invalid_argument("There is no converter for wire schema '"
					+ wireSchema + "'.");
		} else {
			return it->second;
		}

	}

	/**
	 * Accesses a the converter for the specified data type.
	 *
	 * @param dataType data type to find a converter for
	 * @return the converter
	 * @throw std::invalid_argument no converter for the given type
	 */
	Converter getConverterByDataType(const std::string &dataType) const {

		typename std::map<std::string, Converter>::const_iterator it =
				dataTypeMap.find(dataType);
		if (it == dataTypeMap.end()) {
			throw std::invalid_argument("There is no converter for data type '"
					+ dataType + "'.");
		} else {
			return it->second;
		}

	}

	typedef boost::shared_ptr<ConverterCollection<WireFormat> > Ptr;

private:

	std::map<std::string, Converter> wireSchemaMap;
	std::map<std::string, Converter> dataTypeMap;

};

/**
 * @todo how to avoid this central wire format repository?
 */
RSB_EXPORT ConverterCollection<std::string>::Ptr stringConverterCollection();

}
}
