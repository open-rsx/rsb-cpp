#include <rsb/Factory.h>
#include <rsb/converter/Repository.h>
#include <rsb/converter/ProtocolBufferConverter.h>

// See ../CMakeLists.txt for the generation of this file.
// The generated fileund in ${BUILD_DIR}/protobuf_converter
#include <protobuf_converter/SimpleImage.pb.h>

using namespace boost;

using namespace rsb;
using namespace rsb::converter;

// The generated protocol buffer class is in this namespace.
using namespace tutorial::protobuf_converter;

int main() {
    // Register a specific template instanciation of the
    // ProtocolBufferConverter for our SimpleImage protocol buffer
    // message.
    shared_ptr< ProtocolBufferConverter<SimpleImage> >
	converter(new ProtocolBufferConverter<SimpleImage>());
    stringConverterRepository()->registerConverter(converter);

    // Create an informer which has the SimpleImage protocol buffer
    // message as its data type.
    Informer<SimpleImage>::Ptr informer
	= Factory::getInstance().createInformer<SimpleImage>(Scope("/tutorial/converter"));

    // Create and publish an instance of SimpleImage. To see the
    // event, you can, for example use the RSB logger utility.
    Informer<SimpleImage>::DataPtr data(new SimpleImage());
    data->set_width(10);
    data->set_height(10);
    data->set_data(new char[100], 100);
    informer->publish(data);

    return EXIT_SUCCESS;
}
