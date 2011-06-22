
SET(EX1 rsb_informer)
SET(EX2 rsb_listener)
SET(EX3 rsb_reader)

SET(EX4 rsb_sendFile)
SET(EX5 rsb_receiveFile)

SET(EX6 rsb_simpleImageSender)
SET(EX7 rsb_simpleImageSenderNoConverter)

ADD_EXECUTABLE(${EX1} informer/informer.cpp)
ADD_EXECUTABLE(${EX2} listener/listener.cpp)
ADD_EXECUTABLE(${EX3} reader/reader.cpp)

ADD_EXECUTABLE(${EX4} informer/sendFile.cpp)
ADD_EXECUTABLE(${EX5} listener/receiveFile.cpp)

ADD_EXECUTABLE(${EX6} custom_converter/sender.cpp custom_converter/SimpleImageConverter.cpp)
ADD_EXECUTABLE(${EX7} custom_converter/senderNoConverter.cpp)

TARGET_LINK_LIBRARIES(${EX1} ${RSB_NAME})
TARGET_LINK_LIBRARIES(${EX2} ${RSB_NAME})
TARGET_LINK_LIBRARIES(${EX3} ${RSB_NAME})
TARGET_LINK_LIBRARIES(${EX4} ${RSB_NAME})
TARGET_LINK_LIBRARIES(${EX5} ${RSB_NAME})
TARGET_LINK_LIBRARIES(${EX6} ${RSB_NAME})
TARGET_LINK_LIBRARIES(${EX7} ${RSB_NAME})

INSTALL(TARGETS ${EX1} ${EX2} ${EX3} ${EX4} ${EX5} ${EX6} ${EX7}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)

ADD_EXECUTABLE(doublecheck doublecheck.cpp)
TARGET_LINK_LIBRARIES(doublecheck ${RSB_NAME})

ADD_EXECUTABLE(server server/server.cpp)
TARGET_LINK_LIBRARIES(server ${RSB_NAME})
