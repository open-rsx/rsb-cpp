# -*- mode: cmake -*-
cmake_minimum_required(VERSION 3.0)

if(POLICY CMP0053)
    cmake_policy(SET CMP0053 OLD)
endif()

# default version information
set(VERSION_MAJOR   "0" CACHE STRING "Major project version part")
set(VERSION_MINOR   "19" CACHE STRING "Minor project version part")
set(RSB_ABI_VERSION "2")
set(SO_VERSION      "${VERSION_MAJOR}.${VERSION_MINOR}")

project(RSB VERSION "${VERSION_MAJOR}.${VERSION_MINOR}"
            LANGUAGES C CXX)

# --- cmake settings ---

# this must be done in the beginning to adjust the cmake path
find_package(RSC "${VERSION_MAJOR}.${VERSION_MINOR}" REQUIRED)
message(STATUS "RSC version: ${RSC_VERSION}")
message(STATUS "RSC libraries: ${RSC_LIBRARIES}")
message(STATUS "RSC runtime library dirs: ${RSC_RUNTIME_LIBRARY_DIRS}")
message(STATUS "RSC CMake module path: ${RSC_CMAKE_MODULE_PATH}")

set(PROJECT_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}/share/cmake/Modules")
set(CMAKE_MODULE_PATH ${PROJECT_CMAKE_PATH} ${CMAKE_MODULE_PATH} ${RSC_CMAKE_MODULE_PATH})
message(STATUS "Using CMake module path: ${CMAKE_MODULE_PATH}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif()

include(FindPkgConfig)
include(CheckCXXCompilerFlag)
include(InstallFilesRecursive)
include(EnableCoverageReport)
include(EnableSlocCount)
include(GenerateDoxygen)
include(GenerateCppcheck)
include(DefineProjectVersion)
include(CurrentDate)

# finally we can deduce the real project version with the RSC macros
define_project_version(RSB_ ${VERSION_MAJOR} ${VERSION_MINOR} 0 "archive")
current_date(RSB_BUILD_DATE)

set(RSB_NAME                      rsb)
set(VERSION_SUFFIX                "${RSB_VERSION_MAJOR}.${RSB_VERSION_MINOR}")
set(LIB_NAME                      "${RSB_NAME}${VERSION_SUFFIX}")
set(INSTALL_PATH_PREFIX           "${RSB_NAME}${VERSION_SUFFIX}")
set(BINARY_PREFIX                 "${RSB_NAME}-")
set(BINARY_SUFFIX                 "cpp${VERSION_SUFFIX}")
set(RSB_PLUGIN_PATH_SUFFIX        "${INSTALL_PATH_PREFIX}/plugins")
set(RSB_RELATIVE_PLUGIN_DIRECTORY "lib/${RSB_PLUGIN_PATH_SUFFIX}")

set(TOOLS info send)

set(RSB_TEST_NAME                rsbtest_integration)
set(CORE_TEST_NAME               rsbtest_core)
set(FACTORY_TEST_NAME            rsbtest_factory)
set(SOCKETCONNECTOR_TEST_NAME    rsbtest_socket)
set(INPROCESSCONNECTOR_TEST_NAME rsbtest_inprocess)
set(TOPLEVEL_CATCH_TEST_NAME     rsbtest_toplevel_catch)
set(PKGCONFIG_TEST_NAME          rsbtest_pkgconfig)

set(OUTPUT_PATH            "${CMAKE_BINARY_DIR}/build")
set(ARCHIVE_OUTPUT_PATH    ${OUTPUT_PATH})
set(LIBRARY_OUTPUT_PATH    ${OUTPUT_PATH})
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_PATH})

# --- global compiler flags ---

set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib:\$ORIGIN/")

include(PedanticCompilerWarnings)

if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif()

# --- user options ---

option(BUILD_TESTS "Decide if the test suite shall be built or not" ON)
option(WITH_PKGCONFIG_TEST "In case the tests are enabled, decide whether the pkg-config test is created or not." OFF)
option(BUILD_EXAMPLES "Decide if the examples shall be built or not" ON)
option(BUILD_SOCKET_TRANSPORT "Decide if the socket transport shall be built or not" ON)
option(EXPORT_TO_CMAKE_PACKAGE_REGISTRY "If set to ON, RSB will be exported to the CMake user package registry so that downstream projects automatically find the workspace location in find_package calls." OFF)
option(EXPORT_BOOST_DEPENDENCY_TO_PKGCONFIG "Decide if boost dependencies are explicitly exported in the pkg-config file" ON)

# --- dependency handling ---

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS OFF)
add_definitions(-DBOOST_ALL_DYN_LINK)
set(BOOST_COMPONENTS regex date_time program_options system)
set(Boost_USE_VERSION 1.38)
find_package(Boost ${Boost_USE_VERSION} REQUIRED ${BOOST_COMPONENTS})

find_package(Threads REQUIRED)
message(STATUS "Threading library: ${CMAKE_THREAD_LIBS_INIT}")

find_package(ProtocolBuffers REQUIRED)
if(NOT PROTOBUF_PROTOC_EXECUTABLE)
    message(FATAL_ERROR "protoc executable not found")
endif()
if(NOT PROTOBUF_FOUND)
    message(FATAL_ERROR "protocol buffers not found")
endif()

# --- based on the dependencies, find out what to really build ---

set(WITH_SOCKET_TRANSPORT ${BUILD_SOCKET_TRANSPORT})

if(WITH_SOCKET_TRANSPORT)
    message(STATUS "Enabled socket transport")
    add_definitions(-DRSB_WITH_SOCKET_TRANSPORT=)
else()
    message(STATUS "Disabled socket transport")
endif()

include_directories(BEFORE SYSTEM ${LIBS_INCLUDE_DIRS}
                                  ${RSC_INCLUDE_DIRS}
                                  ${Boost_INCLUDE_DIRS}
                                  ${BOOSTTHREADPOOL_INCLUDE_DIRS}
                                  ${BOOSTUUID_INCLUDE_DIRS}
                                  ${PROTOBUF_INCLUDE_DIRS})
include_directories(BEFORE src "${CMAKE_BINARY_DIR}/src")
link_directories(${RSC_LIBRARY_DIRS} ${LIBS_LIBRARY_DIRS})

# --- source code ---

# pre-declare compile flags for using the protocol on windows
set(PROTOCOL_EXPORT "RSB_PROTOCOL_EXPORT=__declspec(dllexport)")
set(PROTOCOL_IMPORT "RSB_PROTOCOL_EXPORT=__declspec(dllimport)")

# in case we are on linux, we never need to take care of exporting or importing
# symbols. So make it easy by one defining a compile flag for everything.
# Special handling in other CMake parts now only needs to take care of windows.
if(NOT WIN32)
    add_definitions("-DRSB_PROTOCOL_EXPORT=")
endif()

add_subdirectory(src)

# for everything we are doing after the main library, we need to import the
# protocol classes
if(WIN32)
    add_definitions("/D${PROTOCOL_IMPORT}")
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
add_subdirectory(apps)

enable_testing()
if(BUILD_TESTS)
    include(ProvideGoogleMock)
    if(GMOCK_AVAILABLE)
        add_subdirectory(test)
    else()
        message(WARNING "Could not build unit tests even though desired because Google Mock could not be installed.")
    endif()
endif()

# --- package ---

set(CPACK_PACKAGE_VERSION_MAJOR ${RSB_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${RSB_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${RSB_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION       "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_PACKAGE_VENDOR        "CoR-Lab Bielefeld University")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")

include(ProvideFlexibleCPack)

# --- decide on export macros for config files ---

if(WIN32)
    set(RSB_PROTOCOL_CFLAGS "/D${PROTOCOL_IMPORT}")
else()
    set(RSB_PROTOCOL_CFLAGS "-DRSB_PROTOCOL_EXPORT=")
endif()

# --- pkgconfig file ---

set(PKG_CONFIG_LIB_DEPS "")
set(PKG_CONFIG_INCLUDE_DEPS "")
if(EXPORT_BOOST_DEPENDENCY_TO_PKGCONFIG)
    # Boost
    foreach(LIB_DIR ${Boost_LIBRARY_DIRS})
        set(PKG_CONFIG_LIB_DEPS "${PKG_CONFIG_LIB_DEPS} -L${LIB_DIR}")
    endforeach()
    foreach(LIB ${Boost_LIBRARIES})
        # WORK-AROUND! - This is necessary due to an upstream bug in cmake,
        # see http://www.cmake.org/Bug/view.php?id=14720
        get_filename_component(BOOST_LIBRARY_NAME "${LIB}" NAME_WE)
        string(REPLACE "lib" "" BOOST_LIBRARY_NAME ${BOOST_LIBRARY_NAME})
        set(PKG_CONFIG_LIB_DEPS "${PKG_CONFIG_LIB_DEPS} -l${BOOST_LIBRARY_NAME}")
    endforeach()
    list(APPEND PKG_CONFIG_INCLUDE_DEPS "-I${Boost_INCLUDE_DIR}")
endif()

# Apple environment, handle different library naming
if(APPLE)
    set(FULL_LIBRARY_NAME "-L\${libdir} -l@LIB_NAME@.@RSB_VERSION_MAJOR@.@RSB_VERSION_MINOR@")
else()
    set(FULL_LIBRARY_NAME "-L\${libdir} -l@LIB_NAME@")
endif()

configure_file(rsb.pc.in rsb.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rsb.pc
        DESTINATION lib/pkgconfig
        RENAME      "${RSB_NAME}${VERSION_SUFFIX}.pc")

# --- cmake config files ---

if(EXPORT_TO_CMAKE_PACKAGE_REGISTRY)
    message(STATUS "Exported RSB to CMake package registry")
    export(PACKAGE RSB)
endif()

configure_file(RSBBuildTreeSettings.cmake.in
               "${CMAKE_BINARY_DIR}/RSBBuildTreeSettings.cmake" @ONLY)

set(LIB_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
if(CMAKE_LINK_LIBRARY_SUFFIX)
    set(LIB_SUFFIX ${CMAKE_LINK_LIBRARY_SUFFIX})
endif()
# convert paths passed into the config files to native cmake syntax to avoid
# invalid string escape sequences on windows (backslashes for path separation)
file(TO_CMAKE_PATH ${RSC_DIR} RSC_DIR_FOR_CONFIG)
configure_file(RSBConfig.cmake.in
               ${CMAKE_BINARY_DIR}/RSBConfig.cmake
               @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/RSBConfig.cmake
        DESTINATION "share/${INSTALL_PATH_PREFIX}")
configure_file(RSBConfigVersion.cmake.in
               ${CMAKE_BINARY_DIR}/RSBConfigVersion.cmake
               @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/RSBConfigVersion.cmake
        DESTINATION "share/${INSTALL_PATH_PREFIX}")

export(TARGETS ${LIB_NAME} FILE "${CMAKE_BINARY_DIR}/RSBDepends.cmake")
install(EXPORT      RSBDepends
        DESTINATION "share/${INSTALL_PATH_PREFIX}")

# --- documentation generation ---

generate_doxygen(VERSION "${RSB_VERSION}" INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/${INSTALL_PATH_PREFIX}")

# --- coverage report ---

enable_coverage_report(TARGETS ${LIB_NAME}
                       TESTS ${RSB_TEST_NAME} ${CORE_TEST_NAME}
                             ${SOCKETCONNECTOR_TEST_NAME}
                             ${INPROCESSCONNECTOR_TEST_NAME}
                       FILTER "*coverage/*" "*/test*")

# --- sloccount ---

enable_sloccount(FOLDERS src test examples apps)

# --- cppcheck ---

generate_cppcheck(SOURCES src test apps examples
                  "${CMAKE_CURRENT_BINARY_DIR}/src" "${CMAKE_CURRENT_BINARY_DIR}/test" "${CMAKE_CURRENT_BINARY_DIR}/apps" "${CMAKE_CURRENT_BINARY_DIR}/examples"
                  ENABLE_IDS style)
