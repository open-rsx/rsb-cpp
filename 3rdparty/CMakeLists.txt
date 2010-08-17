IF(BUILD_TESTS)

    INCLUDE_DIRECTORIES(BEFORE gtest-1.5.0
                               gtest-1.5.0/include
                               gmock-1.5.0
                               gmock-1.5.0/include)

    ADD_LIBRARY(${GMOCK_LIBRARY_NAME} STATIC gtest-1.5.0/src/gtest-all.cc
                                             gmock-1.5.0/src/gmock-all.cc)

ENDIF(BUILD_TESTS)