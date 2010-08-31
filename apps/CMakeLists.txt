
SET(BINNAME1 rsb_monitor)
ADD_EXECUTABLE(${BINNAME1} monitor/RSBMonitor.cpp
                           monitor/mongoose.c)
TARGET_LINK_LIBRARIES(${BINNAME1} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})

INSTALL(TARGETS ${BINNAME1}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
