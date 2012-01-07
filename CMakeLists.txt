# -*- mode: cmake -*-
cmake_minimum_required(VERSION 2.6)

PROJECT(RSB)

# version information
SET(RSB_VERSION_MAJOR "0")
SET(RSB_VERSION_MINOR "6")
SET(RSB_VERSION_PATCH "0")
SET(RSB_VERSION "${RSB_VERSION_MAJOR}.${RSB_VERSION_MINOR}.${RSB_VERSION_PATCH}")
SET(RSB_ABI_VERSION "2")

# --- cmake settings ---

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

# this must be done in the beginning to adjust the cmake path
FIND_PACKAGE(RSC "${RSB_VERSION_MAJOR}.${RSB_VERSION_MINOR}" REQUIRED)
MESSAGE(STATUS "RSC version: ${RSC_VERSION}")
MESSAGE(STATUS "RSC libraries: ${RSC_LIBRARIES}")
MESSAGE(STATUS "RSC runtime library dirs: ${RSC_RUNTIME_LIBRARY_DIRS}")
MESSAGE(STATUS "RSC CMake module path: ${RSC_CMAKE_MODULE_PATH}")

SET(PROJECT_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}/share/cmake/Modules")
SET(CMAKE_MODULE_PATH ${PROJECT_CMAKE_PATH} ${CMAKE_MODULE_PATH} ${RSC_CMAKE_MODULE_PATH})
MESSAGE(STATUS "Using CMake module path: ${CMAKE_MODULE_PATH}")

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
ENDIF()

INCLUDE(FindPkgConfig)
INCLUDE(CheckCXXCompilerFlag)
INCLUDE(InstallFilesRecursive)
INCLUDE(EnableCoverageReport)
INCLUDE(EnableSlocCount)
INCLUDE(GenerateDoxygen)
INCLUDE(GenerateCppcheck)

FIND_PACKAGE(Subversion)
INCLUDE(SubversionIsWc)
SET(RSB_WC_REVISION "archive")
Subversion_IS_WC(${CMAKE_CURRENT_SOURCE_DIR} IS_SVN_VERSION)
IF(Subversion_FOUND AND IS_SVN_VERSION)
    Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} RSB)
    MESSAGE(STATUS "Current revision is ${RSB_WC_REVISION}")
ENDIF()

SET(RSB_NAME rsbcore)
SET(RSB_TEST_NAME rsbtest)
SET(CORE_TEST_NAME rsbtest_core)
SET(SPREADPORT_TEST_NAME rsbtest_spread)
SET(SPREADCONNECTION_TEST_NAME rsbtest_spreadconnection)
SET(INPROCESSCONNECTOR_TEST_NAME rsbtest_inprocess)

SET(OUTPUT_PATH ${CMAKE_BINARY_DIR}/build)
SET(ARCHIVE_OUTPUT_PATH ${OUTPUT_PATH})
SET(LIBRARY_OUTPUT_PATH ${OUTPUT_PATH})
SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_PATH})

# --- global compiler flags ---

INCLUDE(PedanticCompilerWarnings)

IF(WIN32)
    ADD_DEFINITIONS(-D_USE_MATH_DEFINES)
ENDIF()

# --- user options ---

OPTION(BUILD_TESTS "Decide if the test suite shall be built or not" ON)
OPTION(BUILD_EXAMPLES "Decide if the examples shall be built or not" ON)
OPTION(BUILD_SPREAD_TRANSPORT "Decide if the spread transport shall be built or not" ON)
OPTION(BUILD_SOCKET_TRANSPORT "Decide if the socket transport shall be built or not" ON)

# --- project settings ---

SET(GTEST_DIR "${CMAKE_SOURCE_DIR}/3rdparty/gmock/gtest")
SET(GTEST_INCLUDE_DIR "${GTEST_DIR}/include")
SET(GMOCK_DIR "${CMAKE_SOURCE_DIR}/3rdparty/gmock")
SET(GMOCK_INCLUDE_DIR "${GMOCK_DIR}/include")
SET(GMOCK_LIBRARY_NAME "gmock")

# --- dependency handling ---

SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_LIBS OFF)
ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
FIND_PACKAGE(Boost 1.38 REQUIRED regex date_time)
IF(NOT RSC_INTERNAL_BOOST_UUID)
    FIND_PACKAGE(BoostUUID REQUIRED)
ENDIF()

FIND_PACKAGE(Threads REQUIRED)
MESSAGE(STATUS "Threading library: ${CMAKE_THREAD_LIBS_INIT}")

FIND_PACKAGE(ProtocolBuffers REQUIRED)
IF(NOT PROTOBUF_PROTOC_EXECUTABLE)
    MESSAGE(FATAL_ERROR "protoc executable not found")
ENDIF()
IF(NOT PROTOBUF_FOUND)
    MESSAGE(FATAL_ERROR "protocol buffers not found")
ENDIF()

# We abuse the "patch" component of the version to encode the wire
# format version
SET(RSB_PROTOCOL_VERSION "${RSB_VERSION_MAJOR}.${RSB_VERSION_MINOR}.8")
FIND_PACKAGE(RSBProtocol ${RSB_PROTOCOL_VERSION} REQUIRED)

SET(SPREAD_IGNORE_EXECUTABLE TRUE)
FIND_PACKAGE(Spread)
IF(NOT SPREAD_EXECUTABLE)
    MESSAGE(STATUS "Spread executable not found. Unit tests will not be built!")
ENDIF()

INCLUDE_DIRECTORIES(BEFORE SYSTEM ${LIBS_INCLUDE_DIRS}
                                  ${RSC_INCLUDE_DIRS}
                                  ${Boost_INCLUDE_DIRS}
                                  ${BOOSTTHREADPOOL_INCLUDE_DIRS}
                                  ${BOOSTUUID_INCLUDE_DIRS}
                                  ${PROTOBUF_INCLUDE_DIRS}
                                  ${SPREAD_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(BEFORE src ${CMAKE_BINARY_DIR}/src)
LINK_DIRECTORIES(${RSC_LIBRARY_DIRS} ${LIBS_LIBRARY_DIRS})

FIND_PACKAGE(Doxygen)

# --- based on the dependencies, find out what to really build ---

SET(WITH_SPREAD_TRANSPORT FALSE)
IF(BUILD_SPREAD_TRANSPORT AND SPREAD_FOUND)
    SET(WITH_SPREAD_TRANSPORT TRUE)
ENDIF()
SET(WITH_SOCKET_TRANSPORT ${BUILD_SOCKET_TRANSPORT})

IF(WITH_SPREAD_TRANSPORT)
    MESSAGE(STATUS "Enabled spread transport")
    ADD_DEFINITIONS(-DRSB_WITH_SPREAD_TRANSPORT=)
ELSE()
    MESSAGE(STATUS "Disabled spread transport")
ENDIF()
IF(WITH_SOCKET_TRANSPORT)
    MESSAGE(STATUS "Enabled socket transport")
    ADD_DEFINITIONS(-DRSB_WITH_SOCKET_TRANSPORT=)
ELSE()
    MESSAGE(STATUS "Disabled socket transport")
ENDIF()

# --- source code ---

ADD_SUBDIRECTORY(3rdparty)

# first, we need to export the protocol files
IF(WIN32)
    ADD_DEFINITIONS("/DRSB_PROTOCOL_EXPORT=__declspec(dllexport)")
ELSE()
    ADD_DEFINITIONS("-DRSB_PROTOCOL_EXPORT=")
ENDIF()

ADD_SUBDIRECTORY(src)

# for everything else, we now import the protocol files
IF(WIN32)
    REMOVE_DEFINITIONS("/DRSB_PROTOCOL_EXPORT=__declspec(dllexport)")
    ADD_DEFINITIONS("/DRSB_PROTOCOL_EXPORT=__declspec(dllimport)")
ENDIF()

IF(BUILD_EXAMPLES)
    ADD_SUBDIRECTORY(examples)
ENDIF()
ADD_SUBDIRECTORY(apps)

ENABLE_TESTING()
IF(BUILD_TESTS AND SPREAD_EXECUTABLE)
    ADD_SUBDIRECTORY(test)
ENDIF()

# --- decide on export macros for config files ---

IF(WIN32)
    SET(RSB_PROTOCOL_CFLAGS "/DRSB_PROTOCOL_EXPORT=__declspec(dllimport)")
ELSE()
    SET(RSB_PROTOCOL_CFLAGS "-DRSB_PROTOCOL_EXPORT=")
ENDIF()

# --- pkgconfig file ---

CONFIGURE_FILE(rsb.pc.in rsb.pc @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/rsb.pc DESTINATION lib/pkgconfig)

# --- cmake config files ---

EXPORT(PACKAGE RSB)

CONFIGURE_FILE(RSBBuildTreeSettings.cmake.in
               "${CMAKE_BINARY_DIR}/RSBBuildTreeSettings.cmake" @ONLY)

SET(LIB_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
IF(CMAKE_LINK_LIBRARY_SUFFIX)
    SET(LIB_SUFFIX ${CMAKE_LINK_LIBRARY_SUFFIX})
ENDIF()
CONFIGURE_FILE(RSBConfig.cmake.in ${CMAKE_BINARY_DIR}/RSBConfig.cmake @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/RSBConfig.cmake DESTINATION share/rsb)
CONFIGURE_FILE(RSBConfigVersion.cmake.in ${CMAKE_BINARY_DIR}/RSBConfigVersion.cmake @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/RSBConfigVersion.cmake DESTINATION share/rsb)

EXPORT(TARGETS ${RSB_NAME} FILE "${CMAKE_BINARY_DIR}/RSBDepends.cmake")
INSTALL(EXPORT RSBDepends
        DESTINATION "share/rsb")

# --- documentation gneration ---

GENERATE_DOXYGEN(VERSION "${RSB_VERSION}")

# --- coverage ropert ---

ENABLE_COVERAGE_REPORT(TARGETS ${RSB_NAME} TESTS ${RSB_TEST_NAME} ${CORE_TEST_NAME} ${SPREADPORT_TEST_NAME} ${INPROCESSCONNECTOR_TEST_NAME} FILTER "*coverage/*" "*/test*")

# --- sloccount ---

ENABLE_SLOCCOUNT(FOLDERS src test examples apps)

# --- cppcheck ---

GENERATE_CPPCHECK(SOURCES src test apps examples
                  "${CMAKE_CURRENT_BINARY_DIR}/src" "${CMAKE_CURRENT_BINARY_DIR}/test" "${CMAKE_CURRENT_BINARY_DIR}/apps" "${CMAKE_CURRENT_BINARY_DIR}/examples"
		  ENABLE_IDS style)

# --- package ---
INCLUDE(CheckLSBTypes)

SET(CPACK_PACKAGE_VERSION_MAJOR ${RSB_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${RSB_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${RSB_VERSION_PATCH})
SET(VERSION_SUFFIX "${RSB_VERSION_MAJOR}.${RSB_VERSION_MINOR}")

# --------------------------------------------------------------------
# !!! Do not change these two lines !!!
SET(CPACK_PACKAGE_VERSION   "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}${VERSION_SUFFIX}-${CPACK_PACKAGE_VERSION}_${LSB_CODENAME}_${LSB_PROCESSOR_ARCH}")
# --------------------------------------------------------------------

IF(NOT CPACK_GENERATOR)
    SET(CPACK_GENERATOR "TGZ")
    IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        SET(CPACK_GENERATOR "${CPACK_GENERATOR};DEB")
    ENDIF()
ENDIF()
SET(CPACK_PACKAGE_VENDOR "CoR-Lab Bielefeld University")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")

IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    SET(CPACK_DEBIAN_PACKAGE_NAME "rsb${VERSION_SUFFIX}")
    SET(CPACK_DEBIAN_PACKAGE_VERSION ${RSB_VERSION_MAJOR}.${RSB_VERSION_MINOR}.${RSB_VERSION_PATCH})
    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sebastian Wrede (swrede@techfak.uni-bielefeld.de)")
    SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Robotics Service Bus")
    SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    SET(CPACK_DEBIAN_PACKAGE_SECTION "devel")
    SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
    SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libprotoc-dev, rsc${VERSION_SUFFIX}, libboost-regex-dev")
    # build dependencies protobuf-compiler, rsbprotocol
    SET(CPACK_DEBIAN_PACKAGE_SUGGESTS "doxygen, lcov, cppcheck, sloccount, spread (>= 4.0), protobuf-compiler")
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")

INCLUDE(CPack)
