# Create and install plugin library.
SET(PLUGIN_NAME rsbintrospection)
ADD_LIBRARY(${PLUGIN_NAME} SHARED "Plugin.cpp"
                                  "Types.cpp"
                                  "Model.cpp"
                                  "IntrospectionSender.cpp")

TARGET_LINK_LIBRARIES(${PLUGIN_NAME} ${RSC_LIBRARIES}
                                     ${LIB_NAME})
SET_TARGET_PROPERTIES(${PLUGIN_NAME}
                      PROPERTIES
                      VERSION ${SO_VERSION})
IF(WIN32)
    SET_TARGET_PROPERTIES(${PLUGIN_NAME}
                          PROPERTIES
                          COMPILE_DEFINITIONS "${PROTOCOL_IMPORT}")
ENDIF()

INSTALL(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION "${RSB_RELATIVE_PLUGIN_DIRECTORY}" NAMELINK_SKIP
        RUNTIME DESTINATION "bin/${RSB_PLUGIN_PATH_SUFFIX}")
