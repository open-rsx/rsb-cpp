/* ============================================================
 *
 * This file is part of RSB.
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
#include <boost/shared_ptr.hpp>

#include <rsc/runtime/TypeStringTools.h>

#include "../AbstractConverter.h"

namespace rsb {
namespace transport {

namespace detail {

std::string
type_name_to_proto_name(const std::string& type_name) {
  std::string result = ".";
  bool colon = false;
  for (std::string::const_iterator it
	 = type_name.begin(); it != type_name.end(); ++it) {
    if (*it == ':') {
      if (colon) {
	colon = false;
      } else {
	result.push_back('.');
	colon = true;
      }
    } else {
      result.push_back(*it);
    }
  }
  return result;
}

std::string
type_name_to_data_type_name(const std::string& type_name) {
  return "protocol-buffer:" + type_name_to_proto_name(type_name);
}

}

template <typename ProtocolBuffer>
class ProtocolBufferConverter : public rsb::transport::AbstractConverter<std::string> {
public:
  ProtocolBufferConverter();
  virtual
  ~ProtocolBufferConverter();

  std::string
  serialize(const AnnotatedData &data,
	    std::string         &wire);

  AnnotatedData
  deserialize(const std::string &wireType,
	      const std::string &wire);
};

// Implementation

template <typename ProtocolBuffer>
ProtocolBufferConverter<ProtocolBuffer>::ProtocolBufferConverter() :
  AbstractConverter<std::string> (rsc::runtime::typeName<ProtocolBuffer>(),
				  detail::type_name_to_data_type_name(rsc::runtime::typeName<ProtocolBuffer>())) {
}

template <typename ProtocolBuffer>
ProtocolBufferConverter<ProtocolBuffer>::~ProtocolBufferConverter() {
}

template <typename ProtocolBuffer>
std::string
ProtocolBufferConverter<ProtocolBuffer>::serialize(const AnnotatedData &data,
						   std::string         &wireData) {
  assert(data.first == getDataType());

  boost::shared_ptr<ProtocolBuffer> s
    = boost::static_pointer_cast<ProtocolBuffer>(data.second);
  s->SerializeToString(&wireData);
  return getWireSchema();
}

template <typename ProtocolBuffer>
AnnotatedData
ProtocolBufferConverter<ProtocolBuffer>::deserialize(const std::string &wireSchema,
						     const std::string &wireData) {
  assert(wireSchema == getWireSchema());

  boost::shared_ptr<ProtocolBuffer> result(new ProtocolBuffer());
  result->ParseFromString(wireData);
  return std::make_pair(getDataType(), result);
}

}
}
