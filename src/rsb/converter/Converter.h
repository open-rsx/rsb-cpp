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

#pragma once

#include <set>
#include <string>
#include <utility>

#include <boost/shared_ptr.hpp>

#include <rsc/runtime/Printable.h>
#include <rsc/runtime/TypeStringTools.h>

#include "../Event.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace converter {

typedef std::pair<std::string, boost::shared_ptr<void> > AnnotatedData;

#define RSB_TYPE_TAG(T) (reinterpret_cast<T *> (0))

/**
 * @author swrede
 * @author jwienke
 * @tparam WireFormat is the serialization format, uchar, string, binary, ...
 */
template<class WireType>
class Converter: public rsc::runtime::Printable {
public:

    virtual ~Converter() {
    }

    /**
     * Serialized the given domain object to the wire.
     *
     * @param data data to serialize
     * @return the wire schema the data is encoded with
     * @throw SerializationException if the serialization failed
     */
    virtual std::string
    serialize(const AnnotatedData &data, WireType &wire) = 0;

    /**
     * Deserializes a domain object from a wire type.
     *
     * @param wireSchema type of the wire message
     * @param wire the wire containing the date
     * @return the deserialized domain object annotated with its data type name
     * @throw SerializationException if deserializing the message fails
     */
    virtual AnnotatedData deserialize(const std::string &wireSchema,
            const WireType &wire) = 0;

    /**
     * Returns the name of the data type this converter is applicable for.
     *
     * @return name of the data type this converter can be used for
     */
    virtual std::string getDataType() const {
        return dataType;
    }

    /**
     * Returns the name of the wire schema this converter can (de)serialize
     * from/to.
     *
     * @return name of the wire schema from/to this converter can
     *         (de)serialize
     */
    virtual std::string getWireSchema() const {
        return wireSchema;
    }

    typedef boost::shared_ptr<Converter<WireType> > Ptr;

protected:

    /**
     * Creates a new instance of this class with automatic handling for types.
     *
     * @param dataType data type this converter can serialize
     * @param wireSchema wire schema this converter can deserialize
     * @todo this constructor cannot be called anymore for WireType string
     */
    Converter(const std::string &dataType, const std::string &wireSchema) :
        dataType(dataType), wireSchema(wireSchema) {
    }

    /**
     * Creates a new instance of this class with a data type
     * string that is inferred based on the template parameter
     * @a DataType
     *
     * @tparam DataType type of the objects that the converter
     *                  (de)serializes. Use the RSB_TYPE_TAG macro with this.
     * @param wireSchema wire schema from/to this converter can
     *                   (de)serialize.
     */
    template<typename DataType>
    Converter(const std::string &wireSchema, const DataType */*unused*/= 0) :
        dataType(rsc::runtime::typeName<DataType>()), wireSchema(wireSchema) {
    }

private:

    std::string dataType;
    std::string wireSchema;

    std::string getClassName() const {
        return rsc::runtime::typeName(*this);
    }

    void printContents(std::ostream &stream) const {
        stream << "wireType = " << rsc::runtime::typeName<WireType>()
               << ", wireSchema = " << this->wireSchema
               << ", dataType = " << this->dataType;
    }
};

}
}
