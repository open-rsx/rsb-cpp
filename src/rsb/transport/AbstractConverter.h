/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#ifndef ABSTRACTCONVERTER_H_
#define ABSTRACTCONVERTER_H_

#include <set>
#include <string>
#include <utility>

#include <boost/shared_ptr.hpp>

#include "../RSBEvent.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {

typedef std::pair<std::string, boost::shared_ptr<void> > AnnotatedData;

/**
 * @author swrede
 * @author jwienke
 * @tparam WireFormat is the serialization format, uchar, string, binary, ...
 */
template<class WireFormat>
class AbstractConverter {
public:

	virtual ~AbstractConverter() {
	}

	/**
	 * Serialized the given domain object to the wire.
	 *
	 * @param data data to serialize
	 * @throw SerializationException if the serialization failed
	 */
	virtual std::string
	serialize(const AnnotatedData &data, WireFormat &wire) = 0;

	/**
	 * Deserializes a domain object from a wire format.
	 *
	 * @param wireType type of the wire message
	 * @param wire the wire containing the date
	 * @return the deserialized domain object annotated with its data type name
	 * @throw SerializationException if deserializing the message fails
	 */
	virtual AnnotatedData deserialize(const std::string &wireType,
			const WireFormat &wire) = 0;

	/**
	 * Returns the name of the data type this converter is applicable for.
	 *
	 * @return name of the data type this converter can be used for
	 */
	virtual std::string getDataType() const {
		return dataType;
	}

	/**
	 * Returns the name of the wire type this converter can deserialize.
	 *
	 * @return name of the wire format this converter can deserialize
	 */
	virtual std::string getWireType() const {
		return wireType;
	}

	typedef boost::shared_ptr<AbstractConverter<WireFormat> > Ptr;

protected:

	/**
	 * Creates a new instance of this class with automatic handling for types.
	 *
	 * @param dataType data type this converter can serialize
	 * @param wireType wire type this converter can deserialize
	 */
	AbstractConverter(const std::string &dataType, const std::string &wireType) :
		dataType(dataType), wireType(wireType) {
	}

private:

	std::string dataType;
	std::string wireType;

};

}
}

#endif /* ABSTRACTCONVERTER_H_ */
