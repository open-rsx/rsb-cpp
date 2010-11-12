
ENABLE_TESTING()

INCLUDE_DIRECTORIES(BEFORE ../src
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

ADD_TEST(${RSB_TEST_NAME} ${RSB_TEST_NAME})

# --- core test ---

SET(CORE_TEST_SOURCES rsbtest_core.cpp
                      rsb/EventProcessorTest.cpp
                      rsb/HandlerTest.cpp
                      rsb/filter/FilterObserverTest.cpp)

ADD_EXECUTABLE(${CORE_TEST_NAME} ${CORE_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${CORE_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${CORE_TEST_NAME} ${CORE_TEST_NAME})

# --- spreadport test ---

SET(SPREADPORT_TEST_SOURCES rsbtest_spread.cpp
                            rsb/transport/spread/SpreadPortTest.cpp
                            rsb/transport/spread/MembershipManagerTest.cpp
                            rsb/InformerTask.cpp
                            rsb/InformerTask.h)

ADD_EXECUTABLE(${SPREADPORT_TEST_NAME} ${SPREADPORT_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${SPREADPORT_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${SPREADPORT_TEST_NAME} ${SPREADPORT_TEST_NAME})
