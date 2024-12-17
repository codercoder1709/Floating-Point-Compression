#!/bin/bash

# Navigate to the project root directory
cd "$(dirname "$0")"

echo "Starting build and run process for DigitalTwin..."

# Compile library
echo "Compiling library..."
cd lib/
rm -rf build && mkdir build && cd build
cmake .. && make || { echo "Library compilation failed."; exit 1; }
cd ../..
echo "Library compiled successfully."

# Compile test cases
echo "Compiling test cases..."
cd tests/
rm -rf build && mkdir build && cd build
cmake .. && make || { echo "Test cases compilation failed."; exit 1; }
echo "Test cases compiled successfully."

# Run test cases
echo "Running test cases..."
./test_lib || { echo "Test cases execution failed."; exit 1; }
cd ../..

# Compile application
echo "Compiling application..."
cd app/
rm -rf build && mkdir build && cd build
cmake .. && make || { echo "Application compilation failed."; exit 1; }
echo "Application compiled successfully."

# Run application
echo "Running application..."
./main_app || { echo "Application execution failed."; exit 1; }

echo "All steps completed successfully."