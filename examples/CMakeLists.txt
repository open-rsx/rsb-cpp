
SET(EX1 rsb_informer)
SET(EX2 rsb_listener)
SET(EX3 rsb_reader)

SET(EX4 rsb_sendFile)
SET(EX5 rsb_receiveFile)

SET(EX6 rsb_simpleImageSender)
SET(EX7 rsb_simpleImageSenderNoConverter)

SET(EX8 rsb_interTransportInprocessAndSpread)
SET(EX9 rsb_interTransportSpreadOnly)

SET(EX10 rsb_simpleImagePBSender)
SET(EX11 rsb_simpleImagePBReceiver)

SET(EX12 rsb_queueProcessing)

SET(EX13 rsb_metaDataSender)
SET(EX14 rsb_metaDataReceiver)

ADD_EXECUTABLE(${EX1} informer/informer.cpp)
ADD_EXECUTABLE(${EX2} listener/listener.cpp)
ADD_EXECUTABLE(${EX3} reader/reader.cpp)

ADD_EXECUTABLE(${EX4} informer/sendFile.cpp)
ADD_EXECUTABLE(${EX5} listener/receiveFile.cpp)

ADD_EXECUTABLE(${EX6} custom_converter/sender.cpp custom_converter/SimpleImageConverter.cpp)
ADD_EXECUTABLE(${EX7} custom_converter/senderNoConverter.cpp)

ADD_EXECUTABLE(${EX8} inter_transport/inprocessAndSpread.cpp)
ADD_EXECUTABLE(${EX9} inter_transport/spreadOnly.cpp)

# Protocol Buffers as domain objects
PROTOBUF_GENERATE_CPP(protobuf_converter/SimpleImage.pb.cc
                      protobuf_converter/SimpleImage.pb.h
                      PROTOFILES protobuf_converter/SimpleImage.proto
                      PROTOROOT  ${CMAKE_CURRENT_SOURCE_DIR}
                      OUTPATH    ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(${EX10} protobuf_converter/sender.cpp
                       protobuf_converter/SimpleImage.pb.cc)
ADD_EXECUTABLE(${EX11} protobuf_converter/receiver.cpp
                       protobuf_converter/SimpleImage.pb.cc)

ADD_EXECUTABLE(${EX12} queue_processing/queueprocessing.cpp)

ADD_EXECUTABLE(${EX13} meta_data/sender.cpp)
ADD_EXECUTABLE(${EX14} meta_data/receiver.cpp)


SET(EXAMPLES ${EX1} ${EX2} ${EX3} ${EX4} ${EX5} ${EX6} ${EX7} ${EX8} ${EX9} ${EX10} ${EX11} ${EX12} ${EX13} ${EX14})

FOREACH(EXAMPLE ${EXAMPLES})
    TARGET_LINK_LIBRARIES(${EXAMPLE} ${RSB_NAME})
ENDFOREACH()

INSTALL(TARGETS ${EXAMPLES}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)

ADD_EXECUTABLE(doublecheck doublecheck.cpp)
TARGET_LINK_LIBRARIES(doublecheck ${RSB_NAME})

ADD_EXECUTABLE(server server/server.cpp)
TARGET_LINK_LIBRARIES(server ${RSB_NAME})
