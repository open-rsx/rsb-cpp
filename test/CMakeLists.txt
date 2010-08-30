
ENABLE_TESTING()

INCLUDE_DIRECTORIES(BEFORE ../src ${GTEST_INCLUDE_DIR} ${GMOCK_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR} 
    ${CMAKE_INSTALL_PREFIX}/include/opencv
    /vol/nao/releases/cuttingedge/include/alvision 
    /vol/nao/releases/cuttingedge/include/altools 
    /vol/nao/releases/cuttingedge/include/alcore)
# --- RSB (integration) test ---

SET(RSB_TEST_SOURCES RSBTest.cpp  ${GENERIC_TEST_SOURCES})
SET(RSB_TEST_NAME rsb_test)

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
SET(CORE_TEST_NAME rsb_test_core)

ADD_EXECUTABLE(${CORE_TEST_NAME} ${CORE_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${CORE_TEST_NAME}
                      ${LIBNAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${CORE_TEST_NAME} ${CORE_TEST_NAME})

# --- spreadport test ---

SET(SPREADPORT_TEST_SOURCES SpreadPortTest.cpp MembershipManagerTest.cpp ${GENERIC_TEST_SOURCES})
SET(SPREADPORT_TEST_NAME rsb_test_spread)

ADD_EXECUTABLE(${SPREADPORT_TEST_NAME} ${SPREADPORT_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${SPREADPORT_TEST_NAME}
                      ${LIBNAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME})

ADD_TEST(${SPREADPORT_TEST_NAME} ${SPREADPORT_TEST_NAME})


# --- ALImageConverter test ---

SET(ALIMAGE_TEST_SOURCES ALImageConverterTest.cpp ${ALIMAGE_TEST_SOURCES})
SET(ALIMAGE_TEST_NAME rsb_test_alimageconverter)

ADD_EXECUTABLE(${ALIMAGE_TEST_NAME} ${ALIMAGE_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${ALIMAGE_TEST_NAME}
                      ${LIBNAME}
                      ${Boost_LIBRARIES}
                      ${GMOCK_LIBRARY_NAME}
                      /vol/nao/releases/cuttingedge/lib/libcv.so
                      /vol/nao/releases/cuttingedge/lib/libhighgui.so 
                      /vol/nao/releases/cuttingedge/lib/libcxcore.so
                      /vol/nao/releases/cuttingedge/lib/naoqi/libalbase.so
                      /vol/nao/releases/cuttingedge/lib/liblibvision.a)

ADD_TEST(${ALIMAGE_TEST_NAME} ${ALIMAGE_TEST_NAME})
