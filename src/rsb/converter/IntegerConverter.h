/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2016 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
 *
 * ============================================================ */

#pragma once

#include <string>

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include "Converter.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace converter {

/**
 * Base class for integer type converters.
 *
 * @author jmoringe
 */
template <typename T>
class IntegerConverter: public Converter<std::string> {
public:
    IntegerConverter(const std::string& wireSchema)
        : Converter<std::string>(wireSchema, RSB_TYPE_TAG(T)) {
    }

    virtual ~IntegerConverter() {
    }

    std::string serialize(const AnnotatedData& data, std::string& wire) {
        assert(data.first == this->getDataType());

        // Just grab the bytes of *data.second and put them into wire.
        boost::shared_ptr<T> number = boost::static_pointer_cast<T>(data.second);
        wire.resize(sizeof(T));
        for (unsigned int i = 0; i < sizeof(T); ++i) {
            wire[i] = (unsigned char) (*number >> (i * 8ull));
        }
        return getWireSchema();
    }

    AnnotatedData deserialize(const std::string& wireSchema,
                              const std::string& wire) {
        assert(wireSchema == getWireSchema());
        assert(wire.size() == sizeof(T));

        boost::shared_ptr<T> number(new T(0));
        for (unsigned int i = 0; i < sizeof(T); ++i) {
            *number |= ((T) ((unsigned char) wire[i]) << (i * 8ull));
        }
        return make_pair(getDataType(), number);
    }

    typedef boost::shared_ptr< IntegerConverter<T> > Ptr;
};

/**
 * Converter for boost::uint32_t.
 *
 * @author jmoringe
 */
class RSB_EXPORT Uint32Converter: public IntegerConverter<boost::uint32_t> {
public:
    Uint32Converter();
};

/**
 * Converter for boost::uint64_t.
 *
 * @author jmoringe
 */
class RSB_EXPORT Uint64Converter: public IntegerConverter<boost::uint64_t> {
public:
    Uint64Converter();
};

/**
 * Converter for boost::int32_t.
 *
 * @author jmoringe
 */
class RSB_EXPORT Int32Converter: public IntegerConverter<boost::int32_t> {
public:
    Int32Converter();
};

/**
 * Converter for boost::int64_t.
 *
 * @author jmoringe
 */
class RSB_EXPORT Int64Converter: public IntegerConverter<boost::int64_t> {
public:
    Int64Converter();
};

}
}
