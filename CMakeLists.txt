cmake_minimum_required(VERSION 3.10)
project(vottun-qubic-bridge-sc-qubic)

# C++ Standard Config
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Google Test
add_subdirectory(external/googletest)
include_directories(external/googletest/googletest/include)

# Header includes
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/contracts)
include_directories(${PROJECT_SOURCE_DIR}/core/src/contracts)
include_directories(${PROJECT_SOURCE_DIR}/core/src)

# Test cpp
add_executable(QubicOrderContractTests test/QubicOrderContractTest.cpp)

# Link Google Test with test script
target_link_libraries(QubicOrderContractTests gtest gtest_main pthread)

# Discover and load gTests
include(GoogleTest)
gtest_discover_tests(QubicOrderContractTests)
