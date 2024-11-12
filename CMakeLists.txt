cmake_minimum_required(VERSION 3.10)
project(vottun-qubic-bridge-sc-qubic)

# C++ Standard Config
set(CMAKE_CXX_STANDARD 11)

# Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Header includes
include_directories(${PROJECT_SOURCE_DIR}/contracts)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/core/core/src/contracts)
include_directories(${PROJECT_SOURCE_DIR}/core/core/src/platform)

# Test cpp
add_executable(QubicOrderContractTests test/QubicOrderContractTest.cpp)

# Link Google Test with test script
target_link_libraries(QubicOrderContractTests ${GTEST_LIBRARIES} pthread)
