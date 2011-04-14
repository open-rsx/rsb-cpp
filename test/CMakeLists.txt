
ENABLE_TESTING()

# generate these files again to avoid exposing the pb files through libraries
# on windows
PROTOBUF_GENERATE_CPP(PROTO_SOURCES PROTO_HEADERS
                      PROTOFILES ${RSBPROTO_FILES}
                      PROTOROOT ${RSBPROTO_ROOT}
                      OUTPATH ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol)
                      
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
                     rsb/InformerTask.h
                     ${PROTO_HEADERS}
                     ${PROTO_SOURCES})

ADD_EXECUTABLE(${RSB_TEST_NAME} ${RSB_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${RSB_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${RSB_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${RSB_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")

# --- core test ---

SET(CORE_TEST_SOURCES rsbtest_core.cpp
                      rsb/EventProcessingStrategyTest.cpp
                      rsb/HandlerTest.cpp
                      rsb/QualityOfServiceSpecTest.cpp
                      rsb/filter/FilterObserverTest.cpp)

ADD_EXECUTABLE(${CORE_TEST_NAME} ${CORE_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${CORE_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${CORE_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${CORE_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")

# --- spreadport test ---

SET(SPREADPORT_TEST_SOURCES rsbtest_spread.cpp
                            rsb/transport/spread/SpreadConnectorTest.cpp
                            rsb/transport/spread/MembershipManagerTest.cpp
                            rsb/InformerTask.cpp
                            rsb/InformerTask.h
                            ${PROTO_HEADERS}
                            ${PROTO_SOURCES})

ADD_EXECUTABLE(${SPREADPORT_TEST_NAME} ${SPREADPORT_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${SPREADPORT_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${SPREADPORT_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${SPREADPORT_TEST_NAME} "--gtest_output=xml:${TEST_RESULT_DIR}/")

IF(WIN32)
    SET(PATH_STRING "$ENV{PATH};${Boost_LIBRARY_DIRS};${RSC_RUNTIME_LIBRARY_DIRS}")
    # requried for PATH entries with a slash before the semicolon
    STRING(REPLACE "\\;" ";" PATH_STRING "${PATH_STRING}")
    STRING(REPLACE ";" "\\;" PATH_STRING "${PATH_STRING}")
    SET_PROPERTY(TEST ${RSB_TEST_NAME} ${CORE_TEST_NAME} ${SPREADPORT_TEST_NAME}
                 PROPERTY ENVIRONMENT "PATH=${PATH_STRING}")
ENDIF()
