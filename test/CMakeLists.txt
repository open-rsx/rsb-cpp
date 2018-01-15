enable_testing()

include_directories(BEFORE ../src
                           ${CMAKE_CURRENT_BINARY_DIR}
                           ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol
                           ${CMAKE_CURRENT_BINARY_DIR}/../src
                           ${GMOCK_INCLUDE_DIRS}
                           ${CMAKE_CURRENT_BINARY_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(${GMOCK_CFLAGS})

# --- setup generic test infrastructure ---

set(TEST_RESULT_DIR ${CMAKE_BINARY_DIR}/testresults)
set(TEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(TEST_SOCKET_PORT 55555 CACHE STRING "the port to use in socket-transport tests")

configure_file(testconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/testconfig.h)

set(AVAILABLE_TESTS)

function(test NAME)
    set(SOURCES ${ARGN})
    add_executable(${NAME} ${SOURCES})
    target_link_libraries(${NAME}
                          ${LIB_NAME}
                          ${Boost_LIBRARIES}
                          ${GMOCK_LIBRARIES})

    add_test(${NAME}
             ${EXECUTABLE_OUTPUT_PATH}/${NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")
    list(APPEND AVAILABLE_TESTS ${NAME})
    set(AVAILABLE_TESTS "${AVAILABLE_TESTS}" PARENT_SCOPE)
endfunction()

# --- integration test ---

test(${RSB_TEST_NAME}
     rsb/integrationtest.cpp

     rsb/InformerTask.cpp
     rsb/InformerTask.h)

# --- core test ---

protobuf_generate_cpp(TEST_PROTO_SOURCES TEST_PROTO_HEADERS
                      PROTOFILES   rsb/converter/TestMessage.proto
                      OUTPATH      ${CMAKE_CURRENT_BINARY_DIR})

test(${CORE_TEST_NAME}
     ${TEST_PROTO_SOURCES}  ${TEST_PROTO_HEADERS}

     rsbtest_core.cpp

     rsb/EventTest.cpp
     rsb/EventIdTest.cpp
     rsb/HandlerTest.cpp
     rsb/InformerTest.cpp
     rsb/MetaDataTest.cpp
     rsb/ParticipantConfigTest.cpp
     rsb/QualityOfServiceSpecTest.cpp
     rsb/ScopeTest.cpp

     rsb/converter/DefaultConverterTest.cpp
     rsb/converter/EventCollectionsConverterTest.cpp
     rsb/converter/RegexConverterPredicateTest.cpp
     rsb/converter/PredicateConverterListTest.cpp
     rsb/converter/ProtocolBufferConverterLinkingTest.cpp
     rsb/converter/ProtocolBufferConverterTest.cpp
     rsb/converter/TestConverter.cpp
     rsb/converter/TestConverter.h
     rsb/converter/TestConverterTest.cpp

     rsb/filter/TypeFilterTest.cpp
     rsb/filter/CauseFilterTest.cpp

     rsb/eventprocessing/ScopeDispatcher.cpp
     rsb/eventprocessing/ParallelEventReceivingStrategyTest.cpp

     rsb/util/EventQueuePushHandlerTest.cpp
     rsb/util/MD5Test.cpp
     rsb/util/QueuePushHandlerTest.cpp

     rsb/transport/FactoryTest.cpp)

# --- factory test ---
# needs to be externalized to start with a fresh factory instance

test(${FACTORY_TEST_NAME}
     rsb/FactoryTest.cpp)

# --- core death tests ---

set(CORE_DEAT_TEST_SOURCES rsb/eventprocessing/ParallelEventReceivingStrategyDeathTest.cpp)

add_executable(parallelStrategyDeathTest ${CORE_DEAT_TEST_SOURCES})
target_link_libraries(parallelStrategyDeathTest
                      ${LIB_NAME}
                      ${Boost_LIBRARIES})

find_package(PythonInterp)
if(PYTHONINTERP_FOUND)
    add_test(parallelStrategyDeathTestException ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/deathTest.py"
                                               --exe "${EXECUTABLE_OUTPUT_PATH}/parallelStrategyDeathTest"
                                               --xml "${TEST_RESULT_DIR}/rsbtest-parallelStrategyDeathTestException.xml"
                                               --name deathTestException
                                               exception)
    add_test(parallelStrategyDeathTestCatchAll ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/deathTest.py"
                                               --exe "${EXECUTABLE_OUTPUT_PATH}/parallelStrategyDeathTest"
                                               --xml "${TEST_RESULT_DIR}/rsbtest-parallelStrategyDeathTestCatchAll.xml"
                                               --name deathTestCatchAll
                                               catchall)
    list(APPEND AVAILABLE_TESTS parallelStrategyDeathTestException parallelStrategyDeathTestCatchAll)
else()
    message(STATUS "Python interpreter not found, not enabling death tests")
endif()

# --- connector test library ---

set(CONNECTOR_TEST_NAME "connectortest")
add_library(${CONNECTOR_TEST_NAME} STATIC rsb/transport/ConnectorTest.cpp
                                          rsb/transport/ConnectorTest.h
                                          rsb/InformerTask.cpp
                                          rsb/InformerTask.h)
target_link_libraries(${CONNECTOR_TEST_NAME} ${LIB_NAME}
                                             ${GMOCK_LIBRARIES})

# --- inprocess connector test ---

set(INPROCESSCONNECTOR_TEST_SOURCES rsbtest_inprocess.cpp
                                    rsb/transport/inprocess/InProcessConnectorTest.cpp)

add_executable(${INPROCESSCONNECTOR_TEST_NAME} ${INPROCESSCONNECTOR_TEST_SOURCES})
target_link_libraries(${INPROCESSCONNECTOR_TEST_NAME}
                      ${CONNECTOR_TEST_NAME})

add_test(${INPROCESSCONNECTOR_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${INPROCESSCONNECTOR_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")
list(APPEND AVAILABLE_TESTS ${INPROCESSCONNECTOR_TEST_NAME})

# --- socket connector test ---

if(WITH_SOCKET_TRANSPORT)

    set(SOCKETCONNECTOR_TEST_SOURCES rsbtest_socket.cpp
                                     rsb/transport/socket/SocketServerRoutingTest.cpp
                                     rsb/transport/socket/SocketConnectorTest.cpp)

    add_executable(${SOCKETCONNECTOR_TEST_NAME} ${SOCKETCONNECTOR_TEST_SOURCES})
    target_link_libraries(${SOCKETCONNECTOR_TEST_NAME}
                          ${CONNECTOR_TEST_NAME})

    add_test(${SOCKETCONNECTOR_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${SOCKETCONNECTOR_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")
    list(APPEND AVAILABLE_TESTS ${SOCKETCONNECTOR_TEST_NAME})

endif()

# --- toplevel catch test ---

if(WITH_SOCKET_TRANSPORT)
    test(${TOPLEVEL_CATCH_TEST_NAME}
         rsbtest_toplevel_catch.cpp)
endif()

# --- pkg-config test ---

if(UNIX AND WITH_PKGCONFIG_TEST)
    configure_file(pkg-config-test.sh.in "${CMAKE_CURRENT_BINARY_DIR}/pkg-config-test.sh")
    add_test(${PKGCONFIG_TEST_NAME} "${CMAKE_CURRENT_BINARY_DIR}/pkg-config-test.sh")
endif()

# --- windows cleanup ---

if(WIN32)
    set(PATH_STRING "$ENV{PATH};${Boost_LIBRARY_DIRS};${RSC_RUNTIME_LIBRARY_DIRS}")
    # requried for PATH entries with a slash before the semicolon
    string(REPLACE "\\;" ";" PATH_STRING "${PATH_STRING}")
    string(REPLACE ";" "\\;" PATH_STRING "${PATH_STRING}")
    set_property(TEST ${AVAILABLE_TESTS}
                 PROPERTY ENVIRONMENT "PATH=${PATH_STRING}")
endif()
