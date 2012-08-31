IF(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    MESSAGE(FATAL_ERROR "Cannot configure CPack to generate Debian packages on non-linux systems.")
ENDIF()

# To create a proper Debian/Ubuntu package, the following CMake
# options should be used:

#SET(BUILD_TESTS OFF)
#SET(BUILD_EXAMPLES OFF)

#SET(CMAKE_BUILD_TYPE Release)
SET(CPACK_STRIP_FILES "TRUE")

# Actual packaging options

SET(CPACK_GENERATOR "DEB")

SET(CPACK_PACKAGE_VERSION            "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}${CPACK_PACKAGE_REVISION}")
SET(CPACK_DEBIAN_PACKAGE_NAME        "lib${RSB_NAME}${VERSION_SUFFIX}")
SET(CPACK_DEBIAN_PACKAGE_VERSION     "${CPACK_PACKAGE_VERSION}")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER  "Sebastian Wrede <swrede@techfak.uni-bielefeld.de>")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Robotics Service Bus (C++ Implementation)
 C++ library implementing the Robotics Service Bus (RSB), a
 lightweight, extensible, event-driven middleware for robotic system
 and other domain.")
SET(CPACK_DEBIAN_PACKAGE_PRIORITY    "optional")
SET(CPACK_DEBIAN_PACKAGE_SECTION     "devel")
#SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE  "${CMAKE_SYSTEM_PROCESSOR}") # Debian uses different names here
SET(CPACK_DEBIAN_PACKAGE_DEPENDS     "libc6, libprotoc-dev, libboost-regex-dev, librsc${VERSION_SUFFIX}")
SET(CPACK_DEBIAN_PACKAGE_SUGGESTS    "doxygen, lcov, cppcheck, sloccount, protobuf-compiler")
# build dependencies protobuf-compiler, rsbprotocol

# Generate postinst and prerm hooks
SET(PACKAGE_ALT_PRIORITY "80")

SET(POSTINST_SCRIPT      "${CMAKE_CURRENT_BINARY_DIR}/postinst")
SET(PRERM_SCRIPT         "${CMAKE_CURRENT_BINARY_DIR}/prerm")
FILE(WRITE "${POSTINST_SCRIPT}" "#!/bin/sh\n\nset -e\n")
FILE(WRITE "${PRERM_SCRIPT}"    "#!/bin/sh\n\nset -e\n")
FOREACH(TOOL ${TOOLS})
    FILE(APPEND "${POSTINST_SCRIPT}"
                "update-alternatives --install                      \\
                   /usr/bin/${BINARY_PREFIX}${TOOL}                 \\
                   ${BINARY_PREFIX}${TOOL}                          \\
                   /usr/bin/${BINARY_PREFIX}${TOOL}${BINARY_SUFFIX} \\
                   ${PACKAGE_ALT_PRIORITY}\n\n")
    FILE(APPEND "${PRERM_SCRIPT}"
                "update-alternatives --remove                           \\
                   ${BINARY_PREFIX}${TOOL}                              \\
                   /usr/bin/${BINARY_PREFIX}${TOOL}${BINARY_SUFFIX}\n\n")
ENDFOREACH()
EXECUTE_PROCESS(COMMAND chmod 755 "${POSTINST_SCRIPT}" "${PRERM_SCRIPT}")
SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${POSTINST_SCRIPT};${PRERM_SCRIPT}")

SET(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}-${CPACK_DEBIAN_PACKAGE_VERSION}_${CMAKE_SYSTEM_PROCESSOR}")

MESSAGE(STATUS "Debian Package: ${CPACK_DEBIAN_PACKAGE_NAME} (${CPACK_DEBIAN_PACKAGE_VERSION}) [${CPACK_PACKAGE_FILE_NAME}.deb]")
