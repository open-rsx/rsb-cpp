cmake_minimum_required(VERSION 2.6)

SET(PROJECTNAME rsb)
PROJECT(${PROJECTNAME})

# --- cmake settings ---

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

SET(PROJECT_CMAKE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_INSTALL_PREFIX}/share/cmake/Modules)
SET(CMAKE_MODULE_PATH ${PROJECT_CMAKE_PATH} ${CMAKE_MODULE_PATH})

MESSAGE("${CMAKE_MODULE_PATH}")

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
ENDIF()

INCLUDE(FindPkgConfig)
INCLUDE(CheckCXXCompilerFlag)
INCLUDE(InstallFilesRecursive)

SET(RSB_VERSION 0.1.0)
SET(RSB_NAME "rsb")
SET(RSB_TEST_NAME "rsbtest")

# --- global compiler flags ---

CHECK_CXX_COMPILER_FLAG(-pipe CHECK_CXX_FLAG_pipe)
IF(CHECK_CXX_FLAG_pipe)
    ADD_DEFINITIONS(-pipe)
ENDIF()
CHECK_CXX_COMPILER_FLAG(-Wall CHECK_CXX_FLAG_Wall)
IF(CHECK_CXX_FLAG_Wall)
    ADD_DEFINITIONS(-Wall)
ENDIF()
CHECK_CXX_COMPILER_FLAG(-Wextra CHECK_CXX_FLAG_Wextra)
IF(CHECK_CXX_FLAG_Wextra)
    ADD_DEFINITIONS(-Wextra)
ENDIF()
CHECK_CXX_COMPILER_FLAG(-fdiagnostics-show-option CHECK_CXX_FLAG_DIAGNOSTICS)
IF(CHECK_CXX_FLAG_DIAGNOSTICS)
    ADD_DEFINITIONS(-fdiagnostics-show-option)
ENDIF()

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
FIND_PACKAGE(Boost 1.34.1 COMPONENTS thread REQUIRED)
FIND_PACKAGE(BoostUUID REQUIRED)

PKG_CHECK_MODULES(LIBS REQUIRED liblog4cxx)

FIND_PACKAGE(Threads REQUIRED)
MESSAGE(STATUS "Threading library: ${CMAKE_THREAD_LIBS_INIT}")

FIND_PACKAGE(Protobuf REQUIRED)

FIND_PACKAGE(RSBProtocol REQUIRED)

FIND_PACKAGE(Spread REQUIRED)

INCLUDE_DIRECTORIES(SYSTEM ${LIBS_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${BOOSTUUID_INCLUDE_DIRS} ${PROTOBUF_INCLUDE_DIRS} ${SPREAD_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(BEFORE src)
LINK_DIRECTORIES(${LIBS_LIBRARY_DIRS})

# --- generate protocol buffers ---

# the generator function provided by FindProtobuf does not handle project-external proto_path's
# this version fixes the problem and also a subfolder problem, see:
# http://public.kitware.com/Bug/view.php?id=10997
# http://public.kitware.com/Bug/view.php?id=11148
function(RSBPROTOBUF_GENERATE_CPP SRCS HDRS)
  if(NOT ARGN)
    message(SEND_ERROR "Error: PROTOBUF_GENERATE_CPP() called without any proto files")
    return()
  endif(NOT ARGN)

  set(${SRCS})
  set(${HDRS})
  foreach(FIL ${ARGN})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)
    get_filename_component(FIL_EXT ${FIL} NAME)
    
    STRING(REPLACE ${RSBPROTO_ROOT} "" WO_PREFIX ${FIL})
    STRING(REPLACE ${FIL_EXT} "" REL_PATH ${WO_PREFIX})

    # Attention: REL_PATH always begins and terminates with /, do not add extra slashes. This confuses the cmake dependency calculation
    list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.pb.cc")
    list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.pb.h")

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.pb.cc"
             "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.pb.h"
      COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
      # TODO this is the bad hack... the root is hard-coded
      ARGS --cpp_out  ${CMAKE_CURRENT_BINARY_DIR} --proto_path ${RSBPROTO_ROOT} ${ABS_FIL}
      DEPENDS ${ABS_FIL}
      COMMENT "Running C++ protocol buffer compiler on ${FIL}"
      VERBATIM )
  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

RSBPROTOBUF_GENERATE_CPP(PROTO_SOURCES PROTO_HEADERS ${RSBPROTO_FILES})
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

# --- source code ---

ADD_SUBDIRECTORY(3rdparty)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})

SET(BINDIR "${PREFIX}/bin")
SET(INCLUDEDIR "${PREFIX}/include")
SET(MANDIR "${PREFIX}/man")
SET(LIBDIR "${PREFIX}/lib")
SET(LIBNAME RSBCore)
SET(BINNAME1 rsb_monitor)
SET(EX1 rsb_informer)
SET(EX2 rsb_listener)
SET(TESTNAME1 rsb_test_pbuf)
SET(TESTNAME2 rsb_test_converter)
SET(TESTNAME3 rsb_test_subscription)
#SET(TESTNAME4 rsb_test_tasks)
SET(TESTNAME5 rsb_test_qad)
SET(TESTNAME6 rsb_test_inprocess)
SET(TESTNAME7 rsb_test_statustask)
SET(TESTNAME8 rsb_test_taskexec)
SET(TESTNAME9 rsb_test_ep)

# Build package config file
#FILE(READ "pkgconfig.in" PKG_IN)
#STRING(REGEX REPLACE "@prefix@" ${CMAKE_INSTALL_PREFIX} PKG_OUT ${PKG_IN})
#FILE(WRITE ${LIBDIR}/pkgconfig/${PKG_CONF_NAME} ${PKG_OUT})
#MESSAGE("-- Package config file generated and installed")

ENABLE_TESTING()
ADD_SUBDIRECTORY(src)

add_executable (${BINNAME1} apps/monitor/RSBMonitor.cpp
                apps/monitor/mongoose.c)
add_executable (${TESTNAME1} test/PBufTest.cpp)
add_executable (${TESTNAME2} test/ConverterTest.cpp)
add_executable (${TESTNAME3} test/SubscriptionTest.cpp)
#add_executable (${TESTNAME4} test/TaskTest.cpp)
add_executable (${TESTNAME5} test/QueueAndDispatchTest.cpp)
add_executable (${TESTNAME6} test/InProcessTest.cpp)
add_executable (${TESTNAME7} test/StatusTaskTest.cpp)
add_executable (${TESTNAME8} test/TaskExecutorTest.cpp)
add_executable (${TESTNAME9} test/EventProcessorTest.cpp)

add_executable (${EX1} examples/informer/informer.cpp)
add_executable (${EX2} examples/listener/listener.cpp)

target_link_libraries(${BINNAME1} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME1} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME2} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME3} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
#target_link_libraries(${TESTNAME4} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME5} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME6} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME7} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME8} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME9} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${EX1} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${EX2} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})

# --- install ---
INSTALL(TARGETS ${BINNAME1} ${EX1} ${EX2}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
INSTALL_FILES_RECURSIVE(include HEADERS)

IF(BUILD_TESTS)
    ADD_SUBDIRECTORY(test)
ENDIF()
