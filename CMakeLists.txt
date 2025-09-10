cmake_minimum_required(VERSION 3.14)
project(logger_tests)

set(CMAKE_CXX_STANDARD 14)

find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

# Logger library
add_library(logger SHARED
    logger.cpp
    real_file_system.cpp
)

target_include_directories(logger PUBLIC .)

# Test executable
add_executable(logger_tests
    test_logger.cpp
)

target_link_libraries(logger_tests
    logger
    GTest::gtest
    GTest::gmock
    GTest::gtest_main
)

# Enable testing
enable_testing()
add_test(NAME logger_unit_tests COMMAND logger_tests)
