IF(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    MESSAGE(FATAL_ERROR "Cannot configure CPack to generate Debian packages on non-linux systems.")
ENDIF()

# To create a proper Debian/Ubuntu package, the following CMake
# options should be used:

#SET(BUILD_TESTS OFF)
#SET(BUILD_EXAMPLES OFF)

#SET(CMAKE_BUILD_TYPE Release)
SET(CPACK_STRIP_FILES "TRUE")

#set(CPACK_INSTALL_PREFIX "/usr")
set(CPACK_SET_DESTDIR On)
set(CMAKE_INSTALL_PREFIX ${CPACK_INSTALL_PREFIX})

# Actual packaging options

SET(VERSION_SUFFIX "${RSB_VERSION_MAJOR}.${RSB_VERSION_MINOR}")

INCLUDE(CheckLSBTypes)
SET(CPACK_PACKAGE_VERSION   "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}${CPACK_PACKAGE_REVISION}")
SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}${VERSION_SUFFIX}-${CPACK_PACKAGE_VERSION}_${LSB_PROCESSOR_ARCH}")

SET(CPACK_GENERATOR "DEB")

SET(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sebastian Wrede <swrede@techfak.uni-bielefeld.de>")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Robotics Service Bus (C++ Implementatation)
 C++ library implementing the Robotics Service Bus (RSB), a
 lightweight, extensible, event-driven middleware for robotic system
 and other domain.")
SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
SET(CPACK_DEBIAN_PACKAGE_SECTION "devel")
SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libprotoc-dev, rsc${VERSION_SUFFIX}, libboost-regex-dev, spread (>= 4.0)")
# build dependencies protobuf-compiler, rsbprotocol
SET(CPACK_DEBIAN_PACKAGE_SUGGESTS "doxygen, lcov, cppcheck, sloccount, protobuf-compiler")
SET(CPACK_DEBIAN_PACKAGE_NAME "rsb${VERSION_SUFFIX}")
# TODO(jmoringe): CPack does not seem to support conflicts
SET(CPACK_DEBIAN_PACKAGE_CONFLICTS "rsb-spread${VERSION_SUFFIX}")
