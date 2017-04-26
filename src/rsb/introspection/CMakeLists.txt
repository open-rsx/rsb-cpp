# Create and install plugin library.
set(PLUGIN_NAME rsbintrospection)
add_library(${PLUGIN_NAME} SHARED "Plugin.cpp"
                                  "Types.cpp"
                                  "Model.cpp"
                                  "IntrospectionSender.cpp")

target_link_libraries(${PLUGIN_NAME} ${RSC_LIBRARIES}
                                     ${LIB_NAME})
set_target_properties(${PLUGIN_NAME}
                      PROPERTIES
                      VERSION ${SO_VERSION})
if(WIN32)
    set_target_properties(${PLUGIN_NAME}
                          PROPERTIES
                          COMPILE_DEFINITIONS "${PROTOCOL_IMPORT}")
endif()

install(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION "${RSB_RELATIVE_PLUGIN_DIRECTORY}" NAMELINK_SKIP
        RUNTIME DESTINATION "bin/${RSB_PLUGIN_PATH_SUFFIX}")
