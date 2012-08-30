# Protocol Buffers as domain objects
PROTOBUF_GENERATE_CPP(protobuf_converter/SimpleImage.pb.cc
                      protobuf_converter/SimpleImage.pb.h
                      PROTOFILES protobuf_converter/SimpleImage.proto
                      PROTOROOT  ${CMAKE_CURRENT_SOURCE_DIR}
                      OUTPATH    ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

MACRO(EXAMPLE NAME)
    LIST(APPEND EXAMPLES ${BINARY})
    SET(BINARY "example-${NAME}")
    ADD_EXECUTABLE(${BINARY} ${ARGN})
    TARGET_LINK_LIBRARIES(${BINARY} ${RSB_NAME})
ENDMACRO()

EXAMPLE(informer
        informer/informer.cpp)
EXAMPLE(anyinformer
        informer/anyInformer.cpp)
EXAMPLE(timed-informer
        timed_informer/informer.cpp)
EXAMPLE(listener
        listener/listener.cpp)
EXAMPLE(reader
        reader/reader.cpp)

EXAMPLE(send-file
        informer/sendFile.cpp)
EXAMPLE(receive-file
        listener/receiveFile.cpp)

EXAMPLE(simple-image-sender
        custom_converter/sender.cpp
        custom_converter/SimpleImageConverter.cpp)
EXAMPLE(simple-image-sender-no-converter
        custom_converter/senderNoConverter.cpp)

EXAMPLE(inter-transport-inprocess-and-spread
        inter_transport/inprocessAndSpread.cpp)
EXAMPLE(inter-transport-spread-only
        inter_transport/spreadOnly.cpp)

EXAMPLE(simple-image-pb-sender
        protobuf_converter/sender.cpp
        protobuf_converter/SimpleImage.pb.cc)
EXAMPLE(simple-image-pb-receiver
        protobuf_converter/receiver.cpp
        protobuf_converter/SimpleImage.pb.cc)

EXAMPLE(queueProcessing
        queue_processing/queueprocessing.cpp)

EXAMPLE(meta-data-sender
        meta_data/sender.cpp)
EXAMPLE(meta-data-receiver
        meta_data/receiver.cpp)

EXAMPLE(server server/server.cpp)
EXAMPLE(client server/client.cpp)

EXAMPLE(doublecheck doublecheck.cpp)
