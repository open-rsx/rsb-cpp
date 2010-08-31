
SET(EX1 rsb_informer)
SET(EX2 rsb_listener)
SET(EX3 rsb_videosubscriber)

ADD_EXECUTABLE(${EX1} informer/informer.cpp)
ADD_EXECUTABLE(${EX2} listener/listener.cpp)
ADD_EXECUTABLE(${EX3} VideoSender/VideoSubscriber.cpp)

TARGET_LINK_LIBRARIES(${EX1} -ldl ${LIBNAME})
TARGET_LINK_LIBRARIES(${EX2} -ldl ${LIBNAME})
TARGET_LINK_LIBRARIES(${EX3} -ldl ${LIBNAME})

INSTALL(TARGETS ${EX1} ${EX2}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
