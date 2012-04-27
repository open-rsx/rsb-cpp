#pragma once

#include <assert.h>

#include <vector>
#include <iostream>

#include <boost/cstdint.hpp>
#include <boost/integer.hpp>

namespace rosetta {

// Types

typedef boost::uint64_t offset;

typedef std::vector<unsigned char> octetVector;

// Primitive operations

template <unsigned int width>
offset
writeInteger(octetVector&                         destination,
             typename boost::uint_t<width>::least value,
             offset                               offset) {
  for (unsigned int i = 0; i < width / 8; ++i) {
    destination[offset + i] = ((unsigned char*) &value)[i];
  }

  return width / 8;
}

template <unsigned int width>
offset
readInteger(const octetVector&                    source,
            typename boost::uint_t<width>::least& destination,
            offset                                offset) {
  for (unsigned int i = 0; i < width / 8; ++i) {
    ((unsigned char*) &destination)[i] = source[offset + i];
  }
  return width / 8;
}

offset
writeBytes(octetVector&       destination,
           const octetVector& value,
           offset             offset) {
  writeInteger<32>(destination, value.size(), offset);
  for (unsigned int i = 0; i < value.size(); ++i) {
    destination[offset + 4 + i] = value[i];
  }
  return 4 + value.size();
}

offset
readBytes(const octetVector& source,
          octetVector&       destination,
          offset             offset) {
  boost::uint32_t length;
  readInteger<32>(source, length, offset);
  destination.resize(length);
  for (unsigned int i = 0; i < length; ++i) {
    destination[i] = source[offset + 4 + i];
  }
  return 4 + length;
}

// Interface

template <typename Mechanism,
          typename DataHolder>
offset
packedSize(const DataHolder& source);

template <typename Mechanism,
          typename DataHolder>
offset
pack(const DataHolder& source,
     octetVector&      destination,
     offset            start,
     offset            end);

template <typename Mechanism,
          typename DataHolder>
offset
pack(const DataHolder& source,
     std::ostream&     destination,
     offset            start = 0,
     offset            end   = 0) {
  octetVector buffer((start == end) ? packedSize<Mechanism>(source) : (end - start));
  boost::uint64_t length = pack<Mechanism>(source, buffer, 0, 0);
  destination.write((const char*) &buffer[0], length);
  return length;
}

template <typename Mechanism,
          typename DataHolder>
offset
unpack(const octetVector& source,
       DataHolder&        destination,
       offset             start,
       offset             end);

template <typename Mechanism,
          typename DataHolder>
offset
unpack(std::istream& source,
       DataHolder&   destination,
       offset        start,
       offset        end) {
  boost::uint64_t length = end - start;
  octetVector buffer(length);
  source.read((char*) &buffer[0], length);
  boost::uint64_t consumed = unpack<Mechanism>(buffer, destination, 0, 0);
  assert(length == consumed);
  return consumed;
}

}
