cmake_minimum_required(VERSION 3.21)
project(mumu_guard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(mumu_guard main.cpp mutex.h)

include(CTest)

if (BUILD_TESTING)
    find_package(GTest CONFIG QUIET)
    if (NOT GTest_FOUND)
        message(STATUS "GTest not found, fetching it")
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.zip
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        FetchContent_MakeAvailable(googletest)
    endif()

    add_executable(mutex_tests test/mutex_tests.cpp)
    target_link_libraries(mutex_tests PRIVATE GTest::gtest)

    include(GoogleTest)
    gtest_discover_tests(mutex_tests)
endif()
