
IF(BUILD_TESTS)

    INCLUDE_DIRECTORIES(BEFORE ${GMOCK_DIR}
                               ${GMOCK_INCLUDE_DIR}
                               ${GTEST_DIR}
                               ${GTEST_INCLUDE_DIR})

    ADD_LIBRARY(${GMOCK_LIBRARY_NAME} STATIC gmock/gtest/src/gtest-all.cc
                                             gmock/src/gmock-all.cc)

ENDIF(BUILD_TESTS)
