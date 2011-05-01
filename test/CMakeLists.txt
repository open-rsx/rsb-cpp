
ENABLE_TESTING()

SET(TEST_RESULT_DIR ${CMAKE_BINARY_DIR}/testresults)

INCLUDE_DIRECTORIES(BEFORE ../src
                           ${CMAKE_CURRENT_BINARY_DIR}
                           ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol
                           ${CMAKE_CURRENT_BINARY_DIR}/../src
                           ${GTEST_INCLUDE_DIR}
                           ${GMOCK_INCLUDE_DIR}
                           ${CMAKE_CURRENT_BINARY_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR})

# --- setup generic test infrastructure ---

SET(TEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
CONFIGURE_FILE(testconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/testconfig.h)

# --- RSB (integration) test ---

SET(RSB_TEST_SOURCES rsb/RSBTest.cpp
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
                      rsb/eventprocessing/ParallelEventProcessingStrategyTest.cpp
                      rsb/filter/FilterObserverTest.cpp
                      rsb/util/MD5Test.cpp)

ADD_EXECUTABLE(${CORE_TEST_NAME} ${CORE_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${CORE_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${CORE_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${CORE_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")

# --- spread connector test ---

SET(SPREADPORT_TEST_SOURCES rsbtest_spread.cpp
                            rsb/transport/spread/SpreadConnectorTest.cpp
                            rsb/transport/spread/MembershipManagerTest.cpp
                            rsb/InformerTask.cpp
                            rsb/InformerTask.h)

ADD_EXECUTABLE(${SPREADPORT_TEST_NAME} ${SPREADPORT_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${SPREADPORT_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${SPREADPORT_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${SPREADPORT_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")

# --- inprocess connector test ---

SET(INPROCESSCONNECTOR_TEST_SOURCES rsbtest_inprocess.cpp
                            rsb/transport/inprocess/InProcessConnectorTest.cpp
                            rsb/InformerTask.cpp
                            rsb/InformerTask.h)

ADD_EXECUTABLE(${INPROCESSCONNECTOR_TEST_NAME} ${INPROCESSCONNECTOR_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${INPROCESSCONNECTOR_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${INPROCESSCONNECTOR_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${INPROCESSCONNECTOR_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")

IF(WIN32)
    SET(PATH_STRING "$ENV{PATH};${Boost_LIBRARY_DIRS};${RSC_RUNTIME_LIBRARY_DIRS}")
    # requried for PATH entries with a slash before the semicolon
    STRING(REPLACE "\\;" ";" PATH_STRING "${PATH_STRING}")
    STRING(REPLACE ";" "\\;" PATH_STRING "${PATH_STRING}")
    SET_PROPERTY(TEST ${RSB_TEST_NAME} ${CORE_TEST_NAME} ${SPREADPORT_TEST_NAME} ${INPROCESSCONNECTOR_TEST_NAME}
                 PROPERTY ENVIRONMENT "PATH=${PATH_STRING}")
ENDIF()
