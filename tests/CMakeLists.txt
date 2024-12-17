cmake_minimum_required(VERSION 3.10)
project(dataProcessingTests)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the library subdirectory
add_subdirectory(${CMAKE_SOURCE_DIR}/../lib ${CMAKE_BINARY_DIR}/lib)

# Detect if running in a CI environment (like GitHub Actions)
if(DEFINED ENV{GITHUB_WORKSPACE})
    # CI Environment - GitHub Actions
    message(STATUS "Configuring for GitHub Actions CI environment")

    # Set paths to the GoogleTest library and include directories
    set(GTEST_LIB_DIR "$ENV{GITHUB_WORKSPACE}/lib")
    set(GTEST_INCLUDE_DIR "$ENV{GITHUB_WORKSPACE}/include")

    # Include GoogleTest headers
    include_directories(${GTEST_INCLUDE_DIR})

    # Create the test executable
    add_executable(test_lib src/test_lib.cpp)

    # Link against GoogleTest and the dataProcessing library
    target_link_libraries(test_lib PRIVATE
        ${GTEST_LIB_DIR}/libgtest.a
        ${GTEST_LIB_DIR}/libgtest_main.a
        dataProcessing
    )

else()
    # Local Environment
    message(STATUS "Configuring for Local environment")

    # Find GoogleTest package locally
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})

    # Create the test executable
    add_executable(test_lib src/test_lib.cpp)

    # Link against GoogleTest and the dataProcessing library
    target_link_libraries(test_lib PRIVATE GTest::GTest GTest::Main dataProcessing)
endif()

# Add test to CTest
enable_testing()
add_test(NAME DataProcessingTests COMMAND test_lib)
