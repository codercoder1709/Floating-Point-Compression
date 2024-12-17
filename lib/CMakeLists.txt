cmake_minimum_required(VERSION 3.10)
project(dataProcessingLib)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create the static library
add_library(dataProcessing STATIC
    src/dataProcessing.cpp
)

# Add include directories for the library
target_include_directories(dataProcessing PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
