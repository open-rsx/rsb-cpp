/* ============================================================
 *
 * This file is part of RSB.
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <boost/shared_ptr.hpp>

#include <rsc/runtime/TypeStringTools.h>

#include "Converter.h"
#include "rsb/rsbexports.h"

#if defined(RSB_IMPORT_TEMPLATES)
template class __declspec(dllimport) rsb::converter::Converter<std::string>;
#endif

namespace rsb {
namespace converter {

namespace detail {

RSB_EXPORT std::string typeNameToProtoName(const std::string& type_name);

RSB_EXPORT std::string typeNameToWireSchema(const std::string& type_name);

}

template<typename ProtocolBuffer>
class ProtocolBufferConverter: public Converter<std::string> {
public:
    ProtocolBufferConverter();
    virtual
    ~ProtocolBufferConverter();

    std::string
    serialize(const AnnotatedData& data, std::string& wire);

    AnnotatedData
    deserialize(const std::string& wireType, const std::string& wire);
};

// Implementation

template<typename ProtocolBuffer>
ProtocolBufferConverter<ProtocolBuffer>::ProtocolBufferConverter() :
    Converter<std::string> (rsc::runtime::typeName<ProtocolBuffer>(),
                            detail::typeNameToWireSchema(rsc::runtime::typeName<
                                                         ProtocolBuffer>())) {
}

template<typename ProtocolBuffer>
ProtocolBufferConverter<ProtocolBuffer>::~ProtocolBufferConverter() {
}

template<typename ProtocolBuffer>
std::string ProtocolBufferConverter<ProtocolBuffer>::serialize(
    const AnnotatedData& data, std::string& wireData) {
    assert(data.first == getDataType());

    boost::shared_ptr<ProtocolBuffer> s = boost::static_pointer_cast<
        ProtocolBuffer>(data.second);
    s->SerializeToString(&wireData);
    return getWireSchema();
}

template<typename ProtocolBuffer>
AnnotatedData ProtocolBufferConverter<ProtocolBuffer>::deserialize(
    const std::string& wireSchema, const std::string& wireData) {
    assert(wireSchema == getWireSchema());

    boost::shared_ptr<ProtocolBuffer> result(new ProtocolBuffer());
    result->ParseFromString(wireData);
    return std::make_pair(getDataType(), result);
}

}
}
