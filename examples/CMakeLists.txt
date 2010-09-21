
SET(EX1 rsb_informer)
SET(EX2 rsb_listener)

ADD_EXECUTABLE(${EX1} informer/informer.cpp)
ADD_EXECUTABLE(${EX2} listener/listener.cpp)

TARGET_LINK_LIBRARIES(${EX1} -ldl ${RSB_NAME})
TARGET_LINK_LIBRARIES(${EX2} -ldl ${RSB_NAME})

INSTALL(TARGETS ${EX1} ${EX2}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
        
ADD_EXECUTABLE(doublecheck doublecheck.cpp)
TARGET_LINK_LIBRARIES(doublecheck -ldl ${RSB_NAME})
