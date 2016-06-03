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
#include <limits>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include "Converter.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace converter {

/**
 * Base class for floating point type converters.
 *
 * @author jmoringe
 */
template <typename T>
class FloatingPointConverter: public Converter<std::string> {
public:
    FloatingPointConverter(const std::string& wireSchema)
        : Converter<std::string>(wireSchema, RSB_TYPE_TAG(T)) {
        if (!std::numeric_limits<T>::is_iec559) {
            throw std::runtime_error("This converter only works on platforms"
                                     " that represent " + getDataType() +
                                     " values in IEC 559 format which does not"
                                     " seem to the case on this platform"
                                     " according to"
                                     " std::numeric_limits<>::is_iec559");
        }
    }

    virtual ~FloatingPointConverter() {
    }

    std::string serialize(const AnnotatedData& data, std::string& wire) {
        assert(data.first == this->getDataType());

        // Just grab the bytes of *data.second and put them into wire.
        boost::shared_ptr<T> number = boost::static_pointer_cast<T>(data.second);
        wire.resize(sizeof(T));
        for (unsigned int i = 0; i < sizeof(T); ++i) {
            wire[i] = ((char*) number.get())[i];
        }
        return getWireSchema();
    }

    AnnotatedData deserialize(const std::string& wireSchema,
                              const std::string& wire) {
        assert(wireSchema == getWireSchema());
        assert(wire.size() == sizeof(T));

        // Write bytes from wire into a floating point number.
        boost::shared_ptr<T> number(new T(0));
        for (unsigned int i = 0; i < sizeof(T); ++i) {
            ((char*) number.get())[i] = wire[i];
        }
        return make_pair(getDataType(), number);
    }

    typedef boost::shared_ptr< FloatingPointConverter<T> > Ptr;
};

/**
 * Converter for float type.
 *
 * @author jmoringe
 */
RSB_EXPORT class FloatConverter: public FloatingPointConverter<float> {
public:
    FloatConverter();
};

/**
 * Converter for double type.
 *
 * @author jmoringe
 */
RSB_EXPORT class DoubleConverter: public FloatingPointConverter<double> {
public:
    DoubleConverter();
};

}
}
