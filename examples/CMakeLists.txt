
SET(EX1 rsb_informer)
SET(EX2 rsb_listener)
SET(EX3 rsb_sendFile)
SET(EX4 rsb_receiveFile)

ADD_EXECUTABLE(${EX1} informer/informer.cpp)
ADD_EXECUTABLE(${EX2} listener/listener.cpp)
ADD_EXECUTABLE(${EX3} informer/sendFile.cpp)
ADD_EXECUTABLE(${EX4} listener/receiveFile.cpp)

TARGET_LINK_LIBRARIES(${EX1} ${RSB_NAME})
TARGET_LINK_LIBRARIES(${EX2} ${RSB_NAME})
TARGET_LINK_LIBRARIES(${EX3} ${RSB_NAME})
TARGET_LINK_LIBRARIES(${EX4} ${RSB_NAME})

INSTALL(TARGETS ${EX1} ${EX2} ${EX3} ${EX4}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
        
ADD_EXECUTABLE(doublecheck doublecheck.cpp)
TARGET_LINK_LIBRARIES(doublecheck ${RSB_NAME})

ADD_EXECUTABLE(server server/server.cpp)
TARGET_LINK_LIBRARIES(server ${RSB_NAME})
