cmake_minimum_required(VERSION 2.6)

SET(PROJECTNAME RSB)
PROJECT(${PROJECTNAME})

# --- cmake settings ---

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

SET(PROJECT_CMAKE_PATH ${CMAKE_SOURCE_DIR}/cmake)
SET(CMAKE_MODULE_PATH ${PROJECT_CMAKE_PATH} ${CMAKE_MODULE_PATH})

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
ENDIF()

INCLUDE(FindPkgConfig)
INCLUDE(CheckCXXCompilerFlag)

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

# TODO: add a find macro for boost uuid

SET(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost 1.34.1 COMPONENTS thread REQUIRED)
FIND_PACKAGE(BoostUUID REQUIRED)

PKG_CHECK_MODULES(LIBS REQUIRED liblog4cxx)

FIND_PACKAGE(Threads REQUIRED)
MESSAGE(STATUS "Threading library: ${CMAKE_THREAD_LIBS_INIT}")

FIND_PACKAGE(Protobuf REQUIRED)

FIND_PACKAGE(Spread REQUIRED)

INCLUDE_DIRECTORIES(src ${LIBS_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${BOOSTUUID_INCLUDE_DIRS} ${PROTOBUF_INCLUDE_DIRS} ${SPREAD_INCLUDE_DIRS})
LINK_DIRECTORIES(${LIBS_LIBRARY_DIRS})

# --- source code ---

ADD_SUBDIRECTORY(3rdparty)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})

SET(BINDIR "${PREFIX}/bin")
SET(INCLUDEDIR "${PREFIX}/include")
SET(MANDIR "${PREFIX}/man")
SET(LIBDIR "${PREFIX}/lib")
SET(LIBNAME RSBCore)
SET(BINNAME1 rsb_monitor)
SET(EX1 sp_producer)
SET(EX2 sp_consumer)
SET(EX3 rsb_informer)
SET(EX4 rsb_listener)
SET(TESTNAME1 rsb_test_pbuf)
SET(TESTNAME2 rsb_test_converter)
SET(TESTNAME3 rsb_test_subscription)
#SET(TESTNAME4 rsb_test_tasks)
SET(TESTNAME5 rsb_test_qad)
SET(TESTNAME6 rsb_test_inprocess)
SET(TESTNAME7 rsb_test_statustask)
SET(TESTNAME8 rsb_test_taskexec)
SET(TESTNAME9 rsb_test_ep)

#PKG_CHECK_MODULES(LIBRARIES REQUIRED ${REQUIRED_PKGCONFIGS})

SET(SOURCES src/RSBException.cpp 
		    src/RSBVersion.cpp
		    src/util/UUID.cpp
		    src/util/Configuration.cpp
		    src/util/Timer.cpp
		    src/util/Task.cpp
		    src/util/PeriodicTask.cpp
		    src/util/TaskExecutor.cpp
		    src/RSBEvent.cpp
		    src/Subscription.cpp
		    src/MatchAndExecute.cpp
		    src/EventProcessor.cpp
		    src/filter/AbstractFilter.cpp
		    src/filter/ScopeFilter.cpp
		    src/filter/FilterObservable.cpp
		    src/filter/FilterObserver.cpp		   
		    src/protocol/Attachment.pb.cc
		    src/protocol/Notification.pb.cc
		    src/protocol/ProtocolException.cpp				   
		    src/protocol/introspection/PortStateChange.pb.cc
		    src/transport/Port.cpp
		    src/transport/Router.cpp
		    src/transport/AbstractConverter.cpp
		    src/transport/TransportFactory.cpp
		    src/transport/QueueAndDispatchTask.cpp		        
		    src/transport/inprocess/StatusTask.cpp
		    src/transport/inprocess/InProcessPort.cpp
		    src/transport/spread/SpreadGroup.cpp
		    src/transport/spread/SpreadPort.cpp
		    src/transport/spread/SpreadMessage.cpp
		    src/transport/spread/SpreadConnection.cpp
		    src/transport/spread/ReceiverTask.cpp
		    src/introspection/IntrospectionConverter.cpp
		    test/InformerTask.cpp
		    test/BTL2StringConverter.cpp)

SET(HEADERS src/RSBVersion.h
            src/RSBException.h 
		    src/CommException.h
		    src/util/UUID.h		    
		    src/util/Configuration.h
		    src/util/ConfigException.h
		    src/util/Timer.h
		    src/util/Task.h
		    src/util/PeriodicTask.h
		    src/util/TaskExecutor.h
		    src/RSBEvent.h
		    src/Action.h
		    src/Subscription.h
		    src/MatchAndExecute.h
		    src/EventProcessor.h
		    src/filter/FilterActionTypes.h
		    src/filter/AbstractFilter.h
		    src/filter/ScopeFilter.h	
		    src/filter/FilterObservable.h
		    src/filter/FilterObserver.h		   		    	    
		    src/protocol/ProtocolException.h			    
		    src/transport/Port.h
		    src/transport/Router.h
		    src/transport/inprocess/StatusTask.h
		    src/transport/inprocess/InProcessPort.h
		    src/transport/TransportFactory.h
		    src/transport/AbstractConverter.h
		    src/transport/QueueAndDispatchTask.h
		    src/transport/spread/Notification.h		    
		    src/transport/spread/SpreadGroup.h
		    src/transport/spread/SpreadPort.h
		    src/transport/spread/SpreadMessage.h		    
		    src/transport/spread/SpreadConnection.h
		    src/transport/spread/ReceiverTask.h
		    src/introspection/PortStateChange.h
		    src/introspection/IntrospectionConverter.h
		    test/BTL2StringConverter.cpp)

# Build package config file
#FILE(READ "pkgconfig.in" PKG_IN)
#STRING(REGEX REPLACE "@prefix@" ${CMAKE_INSTALL_PREFIX} PKG_OUT ${PKG_IN})
#FILE(WRITE ${LIBDIR}/pkgconfig/${PKG_CONF_NAME} ${PKG_OUT})
#MESSAGE("-- Package config file generated and installed")

ENABLE_TESTING()

ADD_LIBRARY(${LIBNAME} SHARED ${SOURCES})
TARGET_LINK_LIBRARIES(${LIBNAME} ${LIBS_LIBRARIES} ${Boost_LIBRARIES} ${SPREAD_LIBRARIES} ${PROTOBUF_LIBRARIES})


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

add_executable (${EX1} examples/producer/producer.cpp)
add_executable (${EX2} examples/consumer/consumer.cpp)
add_executable (${EX3} examples/informer/informer.cpp)
add_executable (${EX4} examples/listener/listener.cpp)

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
target_link_libraries(${EX3} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${EX4} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})

# executable
INSTALL(TARGETS ${BINNAME1} DESTINATION "${BINDIR}/")
INSTALL(TARGETS ${LIBNAME} DESTINATION ${LIBDIR})
INSTALL(FILES ${HEADERS} DESTINATION "${INCLUDEDIR}/${PROJECTNAME}/")

IF(BUILD_TESTS)
    ADD_SUBDIRECTORY(test)
ENDIF()
