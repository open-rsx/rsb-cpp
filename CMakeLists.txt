# -*- mode: cmake -*-
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(RSB)

# default version information
SET(VERSION_MAJOR   "0" CACHE STRING "Major project version part")
SET(VERSION_MINOR   "9" CACHE STRING "Minor project version part")
SET(RSB_ABI_VERSION "2")
SET(SO_VERSION      "${VERSION_MAJOR}.${VERSION_MINOR}")

# --- cmake settings ---

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

# this must be done in the beginning to adjust the cmake path
FIND_PACKAGE(RSC "${VERSION_MAJOR}.${VERSION_MINOR}" REQUIRED)
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
INCLUDE(DefineProjectVersion)
INCLUDE(CurrentDate)

# finally we can deduce the real project version with the RSC macros
DEFINE_PROJECT_VERSION(RSB_ ${VERSION_MAJOR} ${VERSION_MINOR} 0 "archive")
CURRENT_DATE(RSB_BUILD_DATE)

SET(RSB_NAME            rsb)
SET(VERSION_SUFFIX      "${RSB_VERSION_MAJOR}.${RSB_VERSION_MINOR}")
SET(INSTALL_PATH_PREFIX "${RSB_NAME}${VERSION_SUFFIX}")
SET(BINARY_PREFIX       "${RSB_NAME}-")
SET(BINARY_SUFFIX       "cpp${VERSION_SUFFIX}")

SET(TOOLS info send)

SET(RSB_TEST_NAME                rsbtest_integration)
SET(CORE_TEST_NAME               rsbtest_core)
SET(FACTORY_TEST_NAME            rsbtest_factory)
SET(SOCKETCONNECTOR_TEST_NAME    rsbtest_socket)
SET(INPROCESSCONNECTOR_TEST_NAME rsbtest_inprocess)
SET(TOPLEVEL_CATCH_TEST_NAME     rsbtest_toplevel_catch)
SET(PKGCONFIG_TEST_NAME          rsbtest_pkgconfig)

SET(OUTPUT_PATH            "${CMAKE_BINARY_DIR}/build")
SET(ARCHIVE_OUTPUT_PATH    ${OUTPUT_PATH})
SET(LIBRARY_OUTPUT_PATH    ${OUTPUT_PATH})
SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_PATH})

# --- global compiler flags ---

SET(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib:\$ORIGIN/")

INCLUDE(PedanticCompilerWarnings)

IF(WIN32)
    ADD_DEFINITIONS(-D_USE_MATH_DEFINES)
ENDIF()

# --- user options ---

OPTION(BUILD_TESTS "Decide if the test suite shall be built or not" ON)
OPTION(BUILD_EXAMPLES "Decide if the examples shall be built or not" ON)
OPTION(BUILD_SOCKET_TRANSPORT "Decide if the socket transport shall be built or not" ON)
OPTION(EXPORT_TO_CMAKE_PACKAGE_REGISTRY "If set to ON, RSB will be exported to the CMake user package registry so that downstream projects automatically find the workspace location in find_package calls." OFF)
OPTION(EXPORT_BOOST_DEPENDENCY_TO_PKGCONFIG "Decide if boost dependencies are explicitly exported in the pkg-config file" ON)

# --- dependency handling ---

SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_LIBS OFF)
ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
FIND_PACKAGE(Boost 1.38 REQUIRED regex date_time program_options system)
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

# --- based on the dependencies, find out what to really build ---

SET(WITH_SOCKET_TRANSPORT ${BUILD_SOCKET_TRANSPORT})

IF(WITH_SOCKET_TRANSPORT)
    MESSAGE(STATUS "Enabled socket transport")
    ADD_DEFINITIONS(-DRSB_WITH_SOCKET_TRANSPORT=)
ELSE()
    MESSAGE(STATUS "Disabled socket transport")
ENDIF()

INCLUDE_DIRECTORIES(BEFORE SYSTEM ${LIBS_INCLUDE_DIRS}
                                  ${RSC_INCLUDE_DIRS}
                                  ${Boost_INCLUDE_DIRS}
                                  ${BOOSTTHREADPOOL_INCLUDE_DIRS}
                                  ${BOOSTUUID_INCLUDE_DIRS}
                                  ${PROTOBUF_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(BEFORE src "${CMAKE_BINARY_DIR}/src")
LINK_DIRECTORIES(${RSC_LIBRARY_DIRS} ${LIBS_LIBRARY_DIRS})

# --- source code ---

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
IF(BUILD_TESTS)
    INCLUDE(ProvideGoogleMock)
    IF(GMOCK_AVAILABLE)
        ADD_SUBDIRECTORY(test)
    ELSE()
        MESSAGE(WARNING "Could not build unit tests even though desired because Google Mock could not be installed.")
    ENDIF()
ENDIF()

# --- package ---

SET(CPACK_PACKAGE_VERSION_MAJOR ${RSB_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${RSB_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${RSB_VERSION_PATCH})
SET(CPACK_PACKAGE_VERSION       "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

SET(CPACK_PACKAGE_VENDOR        "CoR-Lab Bielefeld University")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")

INCLUDE(ProvideFlexibleCPack)

# --- decide on export macros for config files ---

IF(WIN32)
    SET(RSB_PROTOCOL_CFLAGS "/DRSB_PROTOCOL_EXPORT=__declspec(dllimport)")
ELSE()
    SET(RSB_PROTOCOL_CFLAGS "-DRSB_PROTOCOL_EXPORT=")
ENDIF()

# --- pkgconfig file ---

SET(PKG_CONFIG_LIB_DEPS "${CMAKE_THREAD_LIBS_INIT}")
SET(PKG_CONFIG_INCLUDE_DEPS "")
IF(EXPORT_BOOST_DEPENDENCY_TO_PKGCONFIG)
    # Boost
    FOREACH(LIB_DIR ${Boost_LIBRARY_DIRS})
        SET(PKG_CONFIG_LIB_DEPS "${PKG_CONFIG_LIB_DEPS} -L${LIB_DIR}")
    ENDFOREACH()
    FOREACH(LIB ${Boost_LIBRARIES})
        SET(PKG_CONFIG_LIB_DEPS "${PKG_CONFIG_LIB_DEPS} ${LIB}")
    ENDFOREACH()
    LIST(APPEND PKG_CONFIG_INCLUDE_DEPS "-I${Boost_INCLUDE_DIR}")
ENDIF()

# Apple environment, handle different library naming
IF(APPLE)
    SET(FULL_LIBRARY_NAME "-L\${libdir} -l@RSB_NAME@.@RSB_VERSION_MAJOR@.@RSB_VERSION_MINOR@")
ELSE()
    SET(FULL_LIBRARY_NAME "\${libdir}/@CMAKE_SHARED_LIBRARY_PREFIX@@RSB_NAME@@CMAKE_SHARED_LIBRARY_SUFFIX@.@RSB_VERSION_MAJOR@.@RSB_VERSION_MINOR@")
ENDIF()

CONFIGURE_FILE(rsb.pc.in rsb.pc @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/rsb.pc
        DESTINATION lib/pkgconfig
        RENAME      "${RSB_NAME}${VERSION_SUFFIX}.pc")

# --- cmake config files ---

IF(EXPORT_TO_CMAKE_PACKAGE_REGISTRY)
    MESSAGE(STATUS "Exported RSB to CMake package registry")
    EXPORT(PACKAGE RSB)
ENDIF()

CONFIGURE_FILE(RSBBuildTreeSettings.cmake.in
               "${CMAKE_BINARY_DIR}/RSBBuildTreeSettings.cmake" @ONLY)

SET(LIB_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
IF(CMAKE_LINK_LIBRARY_SUFFIX)
    SET(LIB_SUFFIX ${CMAKE_LINK_LIBRARY_SUFFIX})
ENDIF()
# converte paths passed into the config files to native cmake syntax to avoid
# invalid string escape sequences on windows (backshlashes for path separation)
FILE(TO_CMAKE_PATH ${RSC_DIR} RSC_DIR_FOR_CONFIG)
CONFIGURE_FILE(RSBConfig.cmake.in
               ${CMAKE_BINARY_DIR}/RSBConfig.cmake
               @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/RSBConfig.cmake
        DESTINATION "share/${INSTALL_PATH_PREFIX}")
CONFIGURE_FILE(RSBConfigVersion.cmake.in
               ${CMAKE_BINARY_DIR}/RSBConfigVersion.cmake
               @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/RSBConfigVersion.cmake
        DESTINATION "share/${INSTALL_PATH_PREFIX}")

EXPORT(TARGETS ${RSB_NAME} FILE "${CMAKE_BINARY_DIR}/RSBDepends.cmake")
INSTALL(EXPORT      RSBDepends
        DESTINATION "share/${INSTALL_PATH_PREFIX}")

# --- documentation generation ---

GENERATE_DOXYGEN(VERSION "${RSB_VERSION}" INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/${INSTALL_PATH_PREFIX}")

# --- coverage report ---

ENABLE_COVERAGE_REPORT(TARGETS ${RSB_NAME}
                       TESTS ${RSB_TEST_NAME} ${CORE_TEST_NAME}
                             ${SOCKETCONNECTOR_TEST_NAME}
                             ${INPROCESSCONNECTOR_TEST_NAME}
                       FILTER "*coverage/*" "*/test*")

# --- sloccount ---

ENABLE_SLOCCOUNT(FOLDERS src test examples apps)

# --- cppcheck ---

GENERATE_CPPCHECK(SOURCES src test apps examples
                  "${CMAKE_CURRENT_BINARY_DIR}/src" "${CMAKE_CURRENT_BINARY_DIR}/test" "${CMAKE_CURRENT_BINARY_DIR}/apps" "${CMAKE_CURRENT_BINARY_DIR}/examples"
                  ENABLE_IDS style)
