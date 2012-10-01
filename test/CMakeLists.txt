ENABLE_TESTING()

INCLUDE_DIRECTORIES(BEFORE ../src
                           ${CMAKE_CURRENT_BINARY_DIR}
                           ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol
                           ${CMAKE_CURRENT_BINARY_DIR}/../src
                           ${GMOCK_INCLUDE_DIRS}
                           ${CMAKE_CURRENT_BINARY_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR})

# --- setup generic test infrastructure ---

SET(TEST_RESULT_DIR ${CMAKE_BINARY_DIR}/testresults)
SET(TEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

SET(TEST_SPREAD_PORT 4816 CACHE STRING "the port of the spread daemon for tests")
SET(TEST_SPREAD_CONFIG_SYMLINK "" CACHE STRING "A symlink pointing to the generated test spread config file. If not empty it will be created and used for the unit tests. This is required to prevent a size limitation bug for the argument in the spread daemon.")
SET(SPREAD_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/spread.conf")

SET(REAL_SPREAD_CONFIG_FILE ${SPREAD_CONFIG_FILE})
IF(TEST_SPREAD_CONFIG_SYMLINK)
    SET(REAL_SPREAD_CONFIG_FILE ${TEST_SPREAD_CONFIG_SYMLINK})
ENDIF()

SET(TEST_SOCKET_PORT 55555 CACHE STRING "the port to use in socket-transport tests")

CONFIGURE_FILE(spread.conf.in ${SPREAD_CONFIG_FILE} @ONLY)
CONFIGURE_FILE(testconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/testconfig.h)

# --- integration test ---

SET(RSB_TEST_SOURCES rsb/integrationtest.cpp
                     rsb/InformerTask.cpp
                     rsb/InformerTask.h)

ADD_EXECUTABLE(${RSB_TEST_NAME} ${RSB_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${RSB_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARIES})

ADD_TEST(${RSB_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${RSB_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")

# --- core test ---

SET(CORE_TEST_SOURCES rsbtest_core.cpp
                      rsb/EventTest.cpp
                      rsb/EventIdTest.cpp
                      rsb/EventQueuePushHandlerTest.cpp
                      rsb/FactoryTest.cpp
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
                      rsb/eventprocessing/ParallelEventReceivingStrategyTest.cpp
                      rsb/util/MD5Test.cpp
                      rsb/transport/FactoryTest.cpp)

ADD_EXECUTABLE(${CORE_TEST_NAME} ${CORE_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${CORE_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARIES})

ADD_TEST(${CORE_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${CORE_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")

# --- core death tests ---

SET(CORE_DEAT_TEST_SOURCES rsb/eventprocessing/ParallelEventReceivingStrategyDeathTest.cpp)

ADD_EXECUTABLE(parallelStrategyDeathTest ${CORE_DEAT_TEST_SOURCES})
TARGET_LINK_LIBRARIES(parallelStrategyDeathTest
                      ${RSB_NAME}
                      ${Boost_LIBRARIES})

SET(DEATH_TESTS)
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
    SET(DEATH_TESTS parallelStrategyDeathTestException parallelStrategyDeathTestCatchAll)
ELSE()
    MESSAGE(STATUS "Python interpreter not found, not enabling death tests")
ENDIF()

# --- connector test library ---

SET(CONNECTOR_TEST_NAME "connectortest")
ADD_LIBRARY(${CONNECTOR_TEST_NAME} STATIC rsb/transport/ConnectorTest.cpp
                                          rsb/transport/ConnectorTest.h
                                          rsb/InformerTask.cpp
                                          rsb/InformerTask.h)
TARGET_LINK_LIBRARIES(${CONNECTOR_TEST_NAME} ${RSB_NAME}
                                             ${GMOCK_LIBRARIES})

# --- spread connector test ---

IF(WITH_SPREAD_TRANSPORT AND SPREAD_EXECUTABLE)
    SET(SPREADCONNECTOR_TEST_SOURCES rsbtest_spread.cpp
                                     rsb/transport/spread/AssemblyTest.cpp
                                     rsb/transport/spread/SpreadConnectionTest.cpp
                                     rsb/transport/spread/SpreadConnectorTest.cpp
                                     rsb/transport/spread/SpreadMessageTest.cpp
                                     rsb/transport/spread/MembershipManagerTest.cpp)

    ADD_EXECUTABLE(${SPREADCONNECTOR_TEST_NAME} ${SPREADCONNECTOR_TEST_SOURCES})
    TARGET_LINK_LIBRARIES(${SPREADCONNECTOR_TEST_NAME}
                          ${CONNECTOR_TEST_NAME})

    ADD_TEST(${SPREADCONNECTOR_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${SPREADCONNECTOR_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")
ENDIF()

# --- inprocess connector test ---

SET(INPROCESSCONNECTOR_TEST_SOURCES rsbtest_inprocess.cpp
                                    rsb/transport/inprocess/InProcessConnectorTest.cpp)

ADD_EXECUTABLE(${INPROCESSCONNECTOR_TEST_NAME} ${INPROCESSCONNECTOR_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${INPROCESSCONNECTOR_TEST_NAME}
                      ${CONNECTOR_TEST_NAME})

ADD_TEST(${INPROCESSCONNECTOR_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${INPROCESSCONNECTOR_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")

# --- windows cleanup ---

IF(WIN32)
    SET(PATH_STRING "$ENV{PATH};${Boost_LIBRARY_DIRS};${RSC_RUNTIME_LIBRARY_DIRS}")
    # requried for PATH entries with a slash before the semicolon
    STRING(REPLACE "\\;" ";" PATH_STRING "${PATH_STRING}")
    STRING(REPLACE ";" "\\;" PATH_STRING "${PATH_STRING}")
    SET_PROPERTY(TEST ${RSB_TEST_NAME} ${CORE_TEST_NAME} ${SPREADCONNECTOR_TEST_NAME} ${INPROCESSCONNECTOR_TEST_NAME} ${DEATH_TESTS}
                 PROPERTY ENVIRONMENT "PATH=${PATH_STRING}")
ENDIF()

# --- spread symlink workaround ---

IF(WITH_SPREAD_TRANSPORT AND TEST_SPREAD_CONFIG_SYMLINK)

    ADD_CUSTOM_COMMAND(TARGET ${RSB_TEST_NAME} PRE_BUILD
                       COMMAND ${CMAKE_COMMAND} -E remove ${TEST_SPREAD_CONFIG_SYMLINK}
                       COMMAND ${CMAKE_COMMAND} -E create_symlink ${SPREAD_CONFIG_FILE} ${TEST_SPREAD_CONFIG_SYMLINK}
                       COMMENT "Creating spread test config symlink")
    ADD_CUSTOM_COMMAND(TARGET ${SPREADCONNECTOR_TEST_NAME} PRE_BUILD
                       COMMAND ${CMAKE_COMMAND} -E remove ${TEST_SPREAD_CONFIG_SYMLINK}
                       COMMAND ${CMAKE_COMMAND} -E create_symlink ${SPREAD_CONFIG_FILE} ${TEST_SPREAD_CONFIG_SYMLINK}
                       COMMENT "Creating spread test config symlink")

ENDIF()
