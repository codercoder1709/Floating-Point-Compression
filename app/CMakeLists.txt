cmake_minimum_required(VERSION 3.10)
project(dataProcessingApp)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the library subdirectory
add_subdirectory(${CMAKE_SOURCE_DIR}/../lib ${CMAKE_BINARY_DIR}/lib)

# Create the application executable
add_executable(main_app src/main.cpp)

# Link against the dataProcessing library
target_link_libraries(main_app PRIVATE dataProcessing)

# Include the headers from the app/include directory
target_include_directories(main_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
