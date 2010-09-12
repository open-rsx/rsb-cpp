
ENABLE_TESTING()

INCLUDE_DIRECTORIES(BEFORE ../src ${CMAKE_CURRENT_BINARY_DIR}/../src ${GTEST_INCLUDE_DIR} ${GMOCK_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR}) 

# --- RSB (integration) test ---

SET(RSB_TEST_SOURCES RSBTest.cpp  ${GENERIC_TEST_SOURCES})

ADD_EXECUTABLE(${RSB_TEST_NAME} ${RSB_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${RSB_TEST_NAME}
                      ${LIBNAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${RSB_TEST_NAME} ${RSB_TEST_NAME})

# --- core test ---

#SET(GENERIC_TEST_SOURCES InformerTask.cpp)
SET(GENERIC_TEST_SOURCES )
SET(CORE_TEST_SOURCES TaskTest.cpp FilterObserverTest.cpp HandlerTest.cpp ${GENERIC_TEST_SOURCES})

ADD_EXECUTABLE(${CORE_TEST_NAME} ${CORE_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${CORE_TEST_NAME}
                      ${LIBNAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${CORE_TEST_NAME} ${CORE_TEST_NAME})

# --- spreadport test ---

SET(SPREADPORT_TEST_SOURCES SpreadPortTest.cpp MembershipManagerTest.cpp ${GENERIC_TEST_SOURCES})

ADD_EXECUTABLE(${SPREADPORT_TEST_NAME} ${SPREADPORT_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${SPREADPORT_TEST_NAME}
                      ${LIBNAME}
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

add_executable (${TESTNAME1} PBufTest.cpp)
add_executable (${TESTNAME2} ConverterTest.cpp)
add_executable (${TESTNAME3} SubscriptionTest.cpp)
#add_executable (${TESTNAME4} TaskTest.cpp)
add_executable (${TESTNAME5} QueueAndDispatchTest.cpp)
add_executable (${TESTNAME6} InProcessTest.cpp)
add_executable (${TESTNAME7} StatusTaskTest.cpp)
add_executable (${TESTNAME8} TaskExecutorTest.cpp)
add_executable (${TESTNAME9} EventProcessorTest.cpp)

target_link_libraries(${TESTNAME1} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME2} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME3} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
#target_link_libraries(${TESTNAME4} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME5} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME6} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME7} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME8} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
target_link_libraries(${TESTNAME9} ${CMAKE_THREAD_LIBS_INIT} -ldl ${LIBNAME})
