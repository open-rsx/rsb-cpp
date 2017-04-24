function(app NAME)
    set(BINARY_NAME "${BINARY_PREFIX}${NAME}${BINARY_SUFFIX}")
    add_executable("${BINARY_NAME}"
                   "${NAME}/main.cpp")
    target_link_libraries("${BINARY_NAME}"
                          ${CMAKE_THREAD_LIBS_INIT}
                          ${LIB_NAME})

    install(TARGETS "${BINARY_NAME}"
            RUNTIME DESTINATION bin)
endfunction()

app(info)
app(send)
