# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build

# Include any dependencies generated for this target.
include lib/CMakeFiles/dataProcessing.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include lib/CMakeFiles/dataProcessing.dir/compiler_depend.make

# Include the progress variables for this target.
include lib/CMakeFiles/dataProcessing.dir/progress.make

# Include the compile flags for this target's objects.
include lib/CMakeFiles/dataProcessing.dir/flags.make

lib/CMakeFiles/dataProcessing.dir/codegen:
.PHONY : lib/CMakeFiles/dataProcessing.dir/codegen

lib/CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.o: lib/CMakeFiles/dataProcessing.dir/flags.make
lib/CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.o: /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/lib/src/dataProcessing.cpp
lib/CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.o: lib/CMakeFiles/dataProcessing.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.o"
	cd /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build/lib && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT lib/CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.o -MF CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.o.d -o CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.o -c /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/lib/src/dataProcessing.cpp

lib/CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.i"
	cd /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build/lib && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/lib/src/dataProcessing.cpp > CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.i

lib/CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.s"
	cd /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build/lib && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/lib/src/dataProcessing.cpp -o CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.s

# Object files for target dataProcessing
dataProcessing_OBJECTS = \
"CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.o"

# External object files for target dataProcessing
dataProcessing_EXTERNAL_OBJECTS =

lib/libdataProcessing.a: lib/CMakeFiles/dataProcessing.dir/src/dataProcessing.cpp.o
lib/libdataProcessing.a: lib/CMakeFiles/dataProcessing.dir/build.make
lib/libdataProcessing.a: lib/CMakeFiles/dataProcessing.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libdataProcessing.a"
	cd /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build/lib && $(CMAKE_COMMAND) -P CMakeFiles/dataProcessing.dir/cmake_clean_target.cmake
	cd /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build/lib && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dataProcessing.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/CMakeFiles/dataProcessing.dir/build: lib/libdataProcessing.a
.PHONY : lib/CMakeFiles/dataProcessing.dir/build

lib/CMakeFiles/dataProcessing.dir/clean:
	cd /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build/lib && $(CMAKE_COMMAND) -P CMakeFiles/dataProcessing.dir/cmake_clean.cmake
.PHONY : lib/CMakeFiles/dataProcessing.dir/clean

lib/CMakeFiles/dataProcessing.dir/depend:
	cd /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/lib /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build/lib /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build/lib/CMakeFiles/dataProcessing.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : lib/CMakeFiles/dataProcessing.dir/depend

