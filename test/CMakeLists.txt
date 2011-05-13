
ENABLE_TESTING()

INCLUDE_DIRECTORIES(BEFORE ../src
                           ${CMAKE_CURRENT_BINARY_DIR}
                           ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol
                           ${CMAKE_CURRENT_BINARY_DIR}/../src
                           ${GTEST_INCLUDE_DIR}
                           ${GMOCK_INCLUDE_DIR}
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

CONFIGURE_FILE(spread.conf.in ${SPREAD_CONFIG_FILE} @ONLY)
CONFIGURE_FILE(testconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/testconfig.h)

# --- RSB (integration) test ---

SET(RSB_TEST_SOURCES rsb/UserLevelTest.cpp
                     rsb/InformerTask.cpp
                     rsb/InformerTask.h)

ADD_EXECUTABLE(${RSB_TEST_NAME} ${RSB_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${RSB_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${RSB_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${RSB_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")

# --- core test ---

SET(CORE_TEST_SOURCES rsbtest_core.cpp
                      rsb/FactoryTest.cpp
                      rsb/HandlerTest.cpp
                      rsb/LocalServiceTest.cpp
                      rsb/ParticipantConfigTest.cpp
                      rsb/QualityOfServiceSpecTest.cpp
                      rsb/ScopeTest.cpp
                      rsb/converter/DefaultConverterTest.cpp
                      rsb/eventprocessing/ParallelEventReceivingStrategyTest.cpp
                      rsb/util/MD5Test.cpp)

ADD_EXECUTABLE(${CORE_TEST_NAME} ${CORE_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${CORE_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${CORE_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${CORE_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")

# --- core death tests ---

SET(CORE_DEAT_TEST_SOURCES rsb/eventprocessing/ParallelEventReceivingStrategyDeathTest.cpp)

ADD_EXECUTABLE(parallelStrategyDeathTest ${CORE_DEAT_TEST_SOURCES})
TARGET_LINK_LIBRARIES(parallelStrategyDeathTest
                      ${RSB_NAME}
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
ELSE()
    MESSAGE(STATUS "Python interpreter not found, not enabling death tests")
ENDIF()

# --- spread connector test ---

SET(SPREADPORT_TEST_SOURCES rsbtest_spread.cpp
                            rsb/transport/ConnectorTest.cpp
                            rsb/transport/ConnectorTest.h
                            rsb/transport/spread/AssemblyTest.cpp
                            rsb/transport/spread/SpreadConnectorTest.cpp
                            rsb/transport/spread/SpreadMessageTest.cpp
                            rsb/transport/spread/MembershipManagerTest.cpp
                            rsb/InformerTask.cpp
                            rsb/InformerTask.h)

ADD_EXECUTABLE(${SPREADPORT_TEST_NAME} ${SPREADPORT_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${SPREADPORT_TEST_NAME}
                      ${RSB_NAME}
                      ${SPREAD_PROTOCOL_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${SPREADPORT_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${SPREADPORT_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")

# --- inprocess connector test ---

SET(INPROCESSCONNECTOR_TEST_SOURCES rsbtest_inprocess.cpp
                                    rsb/transport/ConnectorTest.cpp
                                    rsb/transport/ConnectorTest.h
                                    rsb/transport/inprocess/InProcessConnectorTest.cpp
                                    rsb/InformerTask.cpp
                                    rsb/InformerTask.h)

ADD_EXECUTABLE(${INPROCESSCONNECTOR_TEST_NAME} ${INPROCESSCONNECTOR_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${INPROCESSCONNECTOR_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${INPROCESSCONNECTOR_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${INPROCESSCONNECTOR_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")

SET_TESTS_PROPERTIES(${RSB_TEST_NAME} ${SPREADPORT_TEST_NAME} PROPERTIES RSB_transport_spread_host 4815)

# --- windows cleanup ---

IF(WIN32)
    SET(PATH_STRING "$ENV{PATH};${Boost_LIBRARY_DIRS};${RSC_RUNTIME_LIBRARY_DIRS}")
    # requried for PATH entries with a slash before the semicolon
    STRING(REPLACE "\\;" ";" PATH_STRING "${PATH_STRING}")
    STRING(REPLACE ";" "\\;" PATH_STRING "${PATH_STRING}")
    SET_PROPERTY(TEST ${RSB_TEST_NAME} ${CORE_TEST_NAME} ${SPREADPORT_TEST_NAME} ${INPROCESSCONNECTOR_TEST_NAME}
                 PROPERTY ENVIRONMENT "PATH=${PATH_STRING}")
ENDIF()

# --- spread symlink workaround ---

IF(TEST_SPREAD_CONFIG_SYMLINK)

    ADD_CUSTOM_COMMAND(TARGET ${RSB_TEST_NAME} PRE_BUILD
                       COMMAND ${CMAKE_COMMAND} -E remove ${TEST_SPREAD_CONFIG_SYMLINK}
                       COMMAND ${CMAKE_COMMAND} -E create_symlink ${SPREAD_CONFIG_FILE} ${TEST_SPREAD_CONFIG_SYMLINK}
                       COMMENT "Creating spread test config symlink")
    ADD_CUSTOM_COMMAND(TARGET ${SPREADPORT_TEST_NAME} PRE_BUILD
                       COMMAND ${CMAKE_COMMAND} -E remove ${TEST_SPREAD_CONFIG_SYMLINK}
                       COMMAND ${CMAKE_COMMAND} -E create_symlink ${SPREAD_CONFIG_FILE} ${TEST_SPREAD_CONFIG_SYMLINK}
                       COMMENT "Creating spread test config symlink")

ENDIF()
