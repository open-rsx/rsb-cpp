# -*- mode: cmake -*-
cmake_minimum_required(VERSION 2.6)

PROJECT(RSB)

# --- cmake settings ---

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

# this must be done in the beginning to adjust the cmake path
FIND_PACKAGE(RSC REQUIRED)
MESSAGE(STATUS "RSC libraries: ${RSC_LIBRARIES}")
MESSAGE(STATUS "RSC runtime library dirs: ${RSC_RUNTIME_LIBRARY_DIRS}")
MESSAGE(STATUS "RSC CMake module path: ${RSC_CMAKE_MODULE_PATH}")

SET(PROJECT_CMAKE_PATH ${CMAKE_INSTALL_PREFIX}/share/cmake/Modules)
SET(CMAKE_MODULE_PATH "${PROJECT_CMAKE_PATH}" "${CMAKE_MODULE_PATH}" "${RSC_CMAKE_MODULE_PATH}")
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
INCLUDE(GenerateDoxygen)
INCLUDE(GenerateCppcheck)

SET(RSB_VERSION 0.1.0)
SET(RSB_NAME "RSBCore")
SET(RSB_TEST_NAME "rsbtest")
SET(CORE_TEST_NAME rsbtest_core)
SET(SPREADPORT_TEST_NAME rsbtest_spread)

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

# --- project settings ---

SET(GTEST_DIR ${CMAKE_SOURCE_DIR}/3rdparty/gtest-1.5.0)
SET(GTEST_INCLUDE_DIR ${GTEST_DIR}/include)
SET(GMOCK_DIR ${CMAKE_SOURCE_DIR}/3rdparty/gmock-1.5.0)
SET(GMOCK_INCLUDE_DIR ${GMOCK_DIR}/include)
SET(GMOCK_LIBRARY_NAME "gmock")

# --- dependency handling ---

SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_LIBS OFF)
ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
FIND_PACKAGE(Boost 1.34.1 REQUIRED thread date_time)
FIND_PACKAGE(BoostUUID REQUIRED)

FIND_PACKAGE(Threads REQUIRED)
MESSAGE(STATUS "Threading library: ${CMAKE_THREAD_LIBS_INIT}")

FIND_PACKAGE(ProtocolBuffers REQUIRED)
IF(NOT PROTOBUF_PROTOC_EXECUTABLE)
    MESSAGE(FATAL_ERROR "protoc executable not found")
ENDIF()

FIND_PACKAGE(RSBProtocol REQUIRED)

SET(SPREAD_USE_STATIC TRUE)
FIND_PACKAGE(Spread REQUIRED)

INCLUDE_DIRECTORIES(SYSTEM ${LIBS_INCLUDE_DIRS}
                           ${RSC_INCLUDE_DIRS}
                           ${Boost_INCLUDE_DIRS}
                           ${BOOSTTHREADPOOL_INCLUDE_DIRS}
                           ${BOOSTUUID_INCLUDE_DIRS}
                           ${PROTOBUF_INCLUDE_DIRS}
                           ${SPREAD_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(BEFORE src ${CMAKE_BINARY_DIR}/src)
LINK_DIRECTORIES(${RSC_LIBRARY_DIRS} ${LIBS_LIBRARY_DIRS})

FIND_PACKAGE(Doxygen)

# --- source code ---

ADD_SUBDIRECTORY(3rdparty)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(apps)

ENABLE_TESTING()
IF(BUILD_TESTS)
    ADD_SUBDIRECTORY(test)
ENDIF()

# --- pkgconfig file ---

CONFIGURE_FILE(rsb.pc.in rsb.pc @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/rsb.pc DESTINATION lib/pkgconfig)

# --- cmake config file ---

SET(LIB_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
IF(CMAKE_LINK_LIBRARY_SUFFIX)
    SET(LIB_SUFFIX ${CMAKE_LINK_LIBRARY_SUFFIX})
ENDIF()
CONFIGURE_FILE(RSBConfig.cmake.in ${CMAKE_BINARY_DIR}/RSBConfig.cmake @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/RSBConfig.cmake DESTINATION share/RSB)

# --- documentation gneration ---

GENERATE_DOXYGEN(VERSION "${RSB_VERSION}")

# --- coverage ropert ---

ENABLE_COVERAGE_REPORT(TARGETS ${RSB_NAME} TESTS ${RSB_TEST_NAME} ${CORE_TEST_NAME} ${SPREADPORT_TEST_NAME} FILTER "*coverage/*" "*/test*")
