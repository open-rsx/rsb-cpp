# Protocol Buffers as domain objects
protobuf_generate_cpp(protobuf_converter/SimpleImage.pb.cc
                      protobuf_converter/SimpleImage.pb.h
                      PROTOFILES protobuf_converter/SimpleImage.proto
                      PROTOROOT  ${CMAKE_CURRENT_SOURCE_DIR}
                      OUTPATH    ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

macro(example NAME)
    list(APPEND EXAMPLES ${BINARY})
    set(BINARY "example-${NAME}")
    add_executable(${BINARY} ${ARGN})
    target_link_libraries(${BINARY} ${LIB_NAME})
endmacro()

example(configuration-transports
        configuration-transports.cpp)

example(informer
        informer/informer.cpp)
example(anyinformer
        informer/anyInformer.cpp)
example(timed-informer
        timed_informer/informer.cpp)
example(listener
        listener/listener.cpp)
example(reader
        reader/reader.cpp)

example(send-file
        informer/sendFile.cpp)
example(receive-file
        listener/receiveFile.cpp)

example(simple-image-sender
        custom_converter/sender.cpp
        custom_converter/SimpleImageConverter.cpp)
example(simple-image-sender-no-converter
        custom_converter/senderNoConverter.cpp)

example(inter-transport-inprocess-and-socket
        inter_transport/inprocess-and-socket.cpp)
example(inter-transport-socket-only
        inter_transport/socket-only.cpp)

example(simple-image-pb-sender
        protobuf_converter/sender.cpp
        protobuf_converter/SimpleImage.pb.cc)
example(simple-image-pb-receiver
        protobuf_converter/receiver.cpp
        protobuf_converter/SimpleImage.pb.cc)

example(queueProcessing
        queue_processing/queueprocessing.cpp)

example(meta-data-sender
        meta_data/sender.cpp)
example(meta-data-receiver
        meta_data/receiver.cpp)

example(server server/server.cpp)
example(client server/client.cpp)

example(doublecheck doublecheck.cpp)

add_library(exampleplugin SHARED plugin/Plugin.cpp)
target_link_libraries(exampleplugin ${LIB_NAME})
