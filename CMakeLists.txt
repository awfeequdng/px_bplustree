cmake_minimum_required(VERSION 3.18)
project(px_bplustree)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

set(GOOGLETEST_URL https://github.com/google/googletest.git)
set(GOOGLETEST_TAG release-1.10.0)

FetchContent_Declare(googletest)
if (NOT googletest_POPULATED)
    FetchContent_Populate(googletest
            QUIET                                                   # Don't print verbose output while populating.
            SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/src/googletest        # Source files go here.
            BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/build/googletest      # Build files go here.
            SUBBUILD_DIR ${CMAKE_BINARY_DIR}/_deps/sub/googletest      # Sub-build files go here.
            GIT_REPOSITORY ${GOOGLETEST_URL}                               # Download from GIT_URL.
            GIT_TAG ${GOOGLETEST_TAG}                                      # Checkout the specific GIT_TAG version.
            )

#    set(MESSAGE_QUIET ON)                                                           # Silence third party includes.
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})       # Include the dependency.
#    unset(MESSAGE_QUIET)                                                            # Undo silence.
endif ()

enable_testing()
add_executable(
        bpt_test
        bpt_test.cc
)
target_link_libraries(
        bpt_test
        gtest_main
)
include(GoogleTest)
gtest_discover_tests(bpt_test)