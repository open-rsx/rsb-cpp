set(INFO_NAME info)
set(SEND_NAME send)

add_executable("${BINARY_PREFIX}${INFO_NAME}${BINARY_SUFFIX}"
               info/main.cpp)
target_link_libraries("${BINARY_PREFIX}${INFO_NAME}${BINARY_SUFFIX}"
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${LIB_NAME})

add_executable("${BINARY_PREFIX}${SEND_NAME}${BINARY_SUFFIX}"
               send/main.cpp)
target_link_libraries("${BINARY_PREFIX}${SEND_NAME}${BINARY_SUFFIX}"
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${LIB_NAME})

install(TARGETS "${BINARY_PREFIX}${INFO_NAME}${BINARY_SUFFIX}"
        RUNTIME DESTINATION bin)
install(TARGETS "${BINARY_PREFIX}${SEND_NAME}${BINARY_SUFFIX}"
        RUNTIME DESTINATION bin)
