
SET(EX1 rsb_informer)
SET(EX2 rsb_listener)

ADD_EXECUTABLE(${EX1} informer/informer.cpp)
ADD_EXECUTABLE(${EX2} listener/listener.cpp)

TARGET_LINK_LIBRARIES(${EX1} -ldl ${LIBNAME})
TARGET_LINK_LIBRARIES(${EX2} -ldl ${LIBNAME})

INSTALL(TARGETS ${EX1} ${EX2}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
