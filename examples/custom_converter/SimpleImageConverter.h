#pragma once

#include <rsb/converter/Converter.h>

namespace converter_tutorial {

/** @TODO(jmoringe): document
 *
 *
 * @author jmoringe
 */
class SimpleImageConverter: public rsb::converter::Converter<std::string> {
public:
    SimpleImageConverter();

    std::string serialize(const rsb::converter::AnnotatedData &data, std::string &wire);

    rsb::converter::AnnotatedData deserialize(const std::string &wireSchema, const std::string &wire);
};

}
