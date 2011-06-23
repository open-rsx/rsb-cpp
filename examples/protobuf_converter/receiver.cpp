#include <iostream>

#include <rsb/Factory.h>
#include <rsb/converter/Repository.h>
#include <rsb/converter/ProtocolBufferConverter.h>

// See ../CMakeLists.txt for the generation of this file.
// The generated fileund in ${BUILD_DIR}/protobuf_converter
#include <protobuf_converter/SimpleImage.pb.h>

using namespace std;

using namespace boost;

using namespace rsb;
using namespace rsb::converter;

// The generated protocol buffer class is in this namespace.
using namespace tutorial::protobuf_converter;

void printImage(shared_ptr<SimpleImage> image) {
    cout << "received " << image->width() << "x" << image->height() << " image" << endl;
}

int main() {
    // Register a specific template instanciation of the
    // ProtocolBufferConverter for our SimpleImage protocol buffer
    // message.
    shared_ptr< ProtocolBufferConverter<SimpleImage> >
	converter(new ProtocolBufferConverter<SimpleImage>());
    stringConverterRepository()->registerConverter(converter);

    // Create a listener to receive SimpleImage protocol buffer
    // messages.
    ListenerPtr listener
	= Factory::getInstance().createListener(Scope("/tutorial/converter"));
    listener->addHandler(HandlerPtr(new DataFunctionHandler<SimpleImage>(&printImage)));

    sleep(20);

    return EXIT_SUCCESS;
}
