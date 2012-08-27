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

SET(VERSION_SUFFIX "${RSB_VERSION_MAJOR}.${RSB_VERSION_MINOR}")

SET(CPACK_PACKAGE_VERSION           "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}${CPACK_PACKAGE_REVISION}")
SET(CPACK_DEBIAN_PACKAGE_VERSION    "${CPACK_PACKAGE_VERSION}")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sebastian Wrede <swrede@techfak.uni-bielefeld.de>")
IF(WITH_SPREAD_TRANSPORT)
    SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Robotics Service Bus (C++ Implementatation with builtin Spread transport)
 C++ library implementing the Robotics Service Bus (RSB), a
 lightweight, extensible, event-driven middleware for robotic system
 and other domain.
 .
 This package is built with support for a network transport based on
 the Spread group communication framework.")
ELSE()
    SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Robotics Service Bus (C++ Implementatation)
 C++ library implementing the Robotics Service Bus (RSB), a
 lightweight, extensible, event-driven middleware for robotic system
 and other domain.")
ENDIF()
SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
SET(CPACK_DEBIAN_PACKAGE_SECTION  "devel")
SET(CPACK_DEBIAN_ARCHITECTURE     ${CMAKE_SYSTEM_PROCESSOR})
SET(CPACK_DEBIAN_PACKAGE_DEPENDS  "libc6, libprotoc-dev, rsc${VERSION_SUFFIX}, libboost-regex-dev")
# build dependencies protobuf-compiler, rsbprotocol
SET(CPACK_DEBIAN_PACKAGE_SUGGESTS "doxygen, lcov, cppcheck, sloccount, protobuf-compiler")
IF(WITH_SPREAD_TRANSPORT)
    SET(CPACK_DEBIAN_PACKAGE_NAME      "lib${RSB_NAME}-spread${VERSION_SUFFIX}")
    SET(CPACK_DEBIAN_PACKAGE_SUGGESTS  "${CPACK_DEBIAN_PACKAGE_SUGGESTS}, spread (>= 4.0)")
    # TODO(jmoringe): CPack does not seem to support conflicts
    SET(CPACK_DEBIAN_PACKAGE_CONFLICTS "${RSB_NAME}${VERSION_SUFFIX}")
ELSE()
    SET(CPACK_DEBIAN_PACKAGE_NAME      "lib${RSB_NAME}${VERSION_SUFFIX}")
    # TODO(jmoringe): CPack does not seem to support conflicts
    SET(CPACK_DEBIAN_PACKAGE_CONFLICTS "${RSB_NAME}-spread${VERSION_SUFFIX}")
ENDIF()

SET(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}-${CPACK_DEBIAN_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")