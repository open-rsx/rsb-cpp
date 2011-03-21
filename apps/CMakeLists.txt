
SET(MONITOR_NAME rsb_monitor)
SET(SEND_NAME rsb_send)

# --- monitor ---

ADD_EXECUTABLE(${MONITOR_NAME} monitor/RSBMonitor.cpp
                               monitor/mongoose.c)
TARGET_LINK_LIBRARIES(${MONITOR_NAME} ${CMAKE_THREAD_LIBS_INIT} ${RSB_NAME} -ldl)

# --- send ---

ADD_EXECUTABLE(${SEND_NAME} rsbsend/rsbsend.cpp)
TARGET_LINK_LIBRARIES(${SEND_NAME} ${CMAKE_THREAD_LIBS_INIT} ${RSB_NAME})

# --- install target ---

INSTALL(TARGETS ${MONITOR_NAME} ${SEND_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
