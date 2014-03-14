ENABLE_TESTING()

INCLUDE_DIRECTORIES(BEFORE ../src
                           ${CMAKE_CURRENT_BINARY_DIR}
                           ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol
                           ${CMAKE_CURRENT_BINARY_DIR}/../src
                           ${GMOCK_INCLUDE_DIRS}
                           ${CMAKE_CURRENT_BINARY_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR})

ADD_DEFINITIONS(${GMOCK_CFLAGS})

# --- setup generic test infrastructure ---

SET(TEST_RESULT_DIR ${CMAKE_BINARY_DIR}/testresults)
SET(TEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

SET(TEST_SOCKET_PORT 55555 CACHE STRING "the port to use in socket-transport tests")

CONFIGURE_FILE(testconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/testconfig.h)

SET(AVAILABLE_TESTS)

# --- integration test ---

SET(RSB_TEST_SOURCES rsb/integrationtest.cpp
                     rsb/InformerTask.cpp
                     rsb/InformerTask.h)

ADD_EXECUTABLE(${RSB_TEST_NAME} ${RSB_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${RSB_TEST_NAME}
                      ${LIB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARIES})

ADD_TEST(${RSB_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${RSB_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")
LIST(APPEND AVAILABLE_TESTS ${RSB_TEST_NAME})

# --- core test ---

PROTOBUF_GENERATE_CPP(TEST_PROTO_SOURCES TEST_PROTO_HEADERS
                      PROTOFILES   rsb/converter/TestMessage.proto
                      OUTPATH      ${CMAKE_CURRENT_BINARY_DIR})

SET(CORE_TEST_SOURCES rsbtest_core.cpp
                      rsb/EventTest.cpp
                      rsb/EventIdTest.cpp
                      rsb/HandlerTest.cpp
                      rsb/InformerTest.cpp
                      rsb/LocalServiceTest.cpp
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
                      rsb/eventprocessing/ParallelEventReceivingStrategyTest.cpp
                      rsb/util/EventQueuePushHandlerTest.cpp
                      rsb/util/MD5Test.cpp
                      rsb/util/QueuePushHandlerTest.cpp
                      rsb/transport/FactoryTest.cpp)

ADD_EXECUTABLE(${CORE_TEST_NAME} ${CORE_TEST_SOURCES} ${TEST_PROTO_SOURCES} ${TEST_PROTO_HEADERS})
TARGET_LINK_LIBRARIES(${CORE_TEST_NAME}
                      ${LIB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARIES})

ADD_TEST(${CORE_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${CORE_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")
LIST(APPEND AVAILABLE_TESTS ${CORE_TEST_NAME})

# --- factory test ---
# needs to be externalized to start with a fresh factory instance

SET(FACTORY_TEST_SOURCES rsb/FactoryTest.cpp)

ADD_EXECUTABLE(${FACTORY_TEST_NAME} ${FACTORY_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${FACTORY_TEST_NAME}
                      ${LIB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARIES})

ADD_TEST(${FACTORY_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${FACTORY_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")
LIST(APPEND AVAILABLE_TESTS ${FACTORY_TEST_NAME})

# --- core death tests ---

SET(CORE_DEAT_TEST_SOURCES rsb/eventprocessing/ParallelEventReceivingStrategyDeathTest.cpp)

ADD_EXECUTABLE(parallelStrategyDeathTest ${CORE_DEAT_TEST_SOURCES})
TARGET_LINK_LIBRARIES(parallelStrategyDeathTest
                      ${LIB_NAME}
                      ${Boost_LIBRARIES})

FIND_PACKAGE(PythonInterp)
IF(PYTHONINTERP_FOUND)
    ADD_TEST(parallelStrategyDeathTestException ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/deathTest.py"
                                               --exe "${EXECUTABLE_OUTPUT_PATH}/parallelStrategyDeathTest"
                                               --xml "${TEST_RESULT_DIR}/rsbtest-parallelStrategyDeathTestException.xml"
                                               --name deathTestException
                                               exception)
    ADD_TEST(parallelStrategyDeathTestCatchAll ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/deathTest.py"
                                               --exe "${EXECUTABLE_OUTPUT_PATH}/parallelStrategyDeathTest"
                                               --xml "${TEST_RESULT_DIR}/rsbtest-parallelStrategyDeathTestCatchAll.xml"
                                               --name deathTestCatchAll
                                               catchall)
    LIST(APPEND AVAILABLE_TESTS parallelStrategyDeathTestException parallelStrategyDeathTestCatchAll)
ELSE()
    MESSAGE(STATUS "Python interpreter not found, not enabling death tests")
ENDIF()

# --- connector test library ---

SET(CONNECTOR_TEST_NAME "connectortest")
ADD_LIBRARY(${CONNECTOR_TEST_NAME} STATIC rsb/transport/ConnectorTest.cpp
                                          rsb/transport/ConnectorTest.h
                                          rsb/InformerTask.cpp
                                          rsb/InformerTask.h)
TARGET_LINK_LIBRARIES(${CONNECTOR_TEST_NAME} ${LIB_NAME}
                                             ${GMOCK_LIBRARIES})

# --- inprocess connector test ---

SET(INPROCESSCONNECTOR_TEST_SOURCES rsbtest_inprocess.cpp
                                    rsb/transport/inprocess/InProcessConnectorTest.cpp)

ADD_EXECUTABLE(${INPROCESSCONNECTOR_TEST_NAME} ${INPROCESSCONNECTOR_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${INPROCESSCONNECTOR_TEST_NAME}
                      ${CONNECTOR_TEST_NAME})

ADD_TEST(${INPROCESSCONNECTOR_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${INPROCESSCONNECTOR_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")
LIST(APPEND AVAILABLE_TESTS ${INPROCESSCONNECTOR_TEST_NAME})

# --- socket connector test ---

IF(WITH_SOCKET_TRANSPORT)

    SET(SOCKETCONNECTOR_TEST_SOURCES rsbtest_socket.cpp
                                     rsb/transport/socket/SocketServerRoutingTest.cpp
                                     rsb/transport/socket/SocketConnectorTest.cpp)

    ADD_EXECUTABLE(${SOCKETCONNECTOR_TEST_NAME} ${SOCKETCONNECTOR_TEST_SOURCES})
    TARGET_LINK_LIBRARIES(${SOCKETCONNECTOR_TEST_NAME}
                          ${CONNECTOR_TEST_NAME})

    ADD_TEST(${SOCKETCONNECTOR_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${SOCKETCONNECTOR_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")
    LIST(APPEND AVAILABLE_TESTS ${SOCKETCONNECTOR_TEST_NAME})

ENDIF()

# --- toplevel catch test ---


IF(WITH_SOCKET_TRANSPORT)

    SET(TOPLEVEL_CATCH_TEST_SOURCES rsbtest_toplevel_catch)
    ADD_EXECUTABLE(${TOPLEVEL_CATCH_TEST_NAME} ${TOPLEVEL_CATCH_TEST_SOURCES})
    TARGET_LINK_LIBRARIES(${TOPLEVEL_CATCH_TEST_NAME}
                          ${LIB_NAME}
                          ${Boost_LIBRARIES}
                          ${GMOCK_LIBRARIES})

    ADD_TEST(${TOPLEVEL_CATCH_TEST_NAME}
             ${EXECUTABLE_OUTPUT_PATH}/${TOPLEVEL_CATCH_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")
    LIST(APPEND AVAILABLE_TESTS ${TOPLEVEL_CATCH_TEST_NAME})

ENDIF()

# --- pkg-config test ---
IF(UNIX AND WITH_PKGCONFIG_TEST)
    CONFIGURE_FILE(pkg-config-test.sh.in "${CMAKE_CURRENT_BINARY_DIR}/pkg-config-test.sh")
    ADD_TEST(${PKGCONFIG_TEST_NAME} "${CMAKE_CURRENT_BINARY_DIR}/pkg-config-test.sh")
ENDIF()

# --- windows cleanup ---

IF(WIN32)
    SET(PATH_STRING "$ENV{PATH};${Boost_LIBRARY_DIRS};${RSC_RUNTIME_LIBRARY_DIRS}")
    # requried for PATH entries with a slash before the semicolon
    STRING(REPLACE "\\;" ";" PATH_STRING "${PATH_STRING}")
    STRING(REPLACE ";" "\\;" PATH_STRING "${PATH_STRING}")
    SET_PROPERTY(TEST ${AVAILABLE_TESTS}
                 PROPERTY ENVIRONMENT "PATH=${PATH_STRING}")
ENDIF()
