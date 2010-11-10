
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

SET(CORE_TEST_SOURCES rsb/filter/FilterObserverTest.cpp
                      rsb/util/TaskTest.cpp
                      rsb/HandlerTest.cpp)

ADD_EXECUTABLE(${CORE_TEST_NAME} ${CORE_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${CORE_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${CORE_TEST_NAME} ${CORE_TEST_NAME})

# --- spreadport test ---

SET(SPREADPORT_TEST_SOURCES rsb/transport/spread/SpreadPortTest.cpp
                            rsb/transport/spread/MembershipManagerTest.cpp
                            rsb/InformerTask.cpp
                            rsb/InformerTask.h)

ADD_EXECUTABLE(${SPREADPORT_TEST_NAME} ${SPREADPORT_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${SPREADPORT_TEST_NAME}
                      ${RSB_NAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${SPREADPORT_TEST_NAME} ${SPREADPORT_TEST_NAME})

# --- TODO old stuff ---

SET(TESTNAME1 rsb_test_pbuf)
SET(TESTNAME2 rsb_test_converter)
SET(TESTNAME3 rsb_test_subscription)
#SET(TESTNAME4 rsb_test_tasks)
SET(TESTNAME5 rsb_test_qad)
SET(TESTNAME6 rsb_test_inprocess)
SET(TESTNAME7 rsb_test_statustask)
SET(TESTNAME8 rsb_test_taskexec)
SET(TESTNAME9 rsb_test_ep)

#ADD_EXECUTABLE(${TESTNAME1} PBufTest.cpp)
#ADD_EXECUTABLE(${TESTNAME2} ConverterTest.cpp)
#ADD_EXECUTABLE(${TESTNAME3} SubscriptionTest.cpp)
#ADD_EXECUTABLE(${TESTNAME5} QueueAndDispatchTest.cpp)
#ADD_EXECUTABLE(${TESTNAME6} InProcessTest.cpp)
#ADD_EXECUTABLE(${TESTNAME7} StatusTaskTest.cpp)
#ADD_EXECUTABLE(${TESTNAME8} TaskExecutorTest.cpp)
#ADD_EXECUTABLE(${TESTNAME9} EventProcessorTest.cpp)

#TARGET_LINK_LIBRARIES(${TESTNAME1} ${CMAKE_THREAD_LIBS_INIT} -ldl ${RSB_NAME})
#TARGET_LINK_LIBRARIES(${TESTNAME2} ${CMAKE_THREAD_LIBS_INIT} -ldl ${RSB_NAME})
#TARGET_LINK_LIBRARIES(${TESTNAME3} ${CMAKE_THREAD_LIBS_INIT} -ldl ${RSB_NAME})
#TARGET_LINK_LIBRARIES(${TESTNAME5} ${CMAKE_THREAD_LIBS_INIT} -ldl ${RSB_NAME})
#TARGET_LINK_LIBRARIES(${TESTNAME6} ${CMAKE_THREAD_LIBS_INIT} -ldl ${RSB_NAME})
#TARGET_LINK_LIBRARIES(${TESTNAME7} ${CMAKE_THREAD_LIBS_INIT} -ldl ${RSB_NAME})
#TARGET_LINK_LIBRARIES(${TESTNAME8} ${CMAKE_THREAD_LIBS_INIT} -ldl ${RSB_NAME})
#TARGET_LINK_LIBRARIES(${TESTNAME9} ${CMAKE_THREAD_LIBS_INIT} -ldl ${RSB_NAME})
