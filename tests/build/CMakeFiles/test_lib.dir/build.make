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
include CMakeFiles/test_lib.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/test_lib.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/test_lib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_lib.dir/flags.make

CMakeFiles/test_lib.dir/codegen:
.PHONY : CMakeFiles/test_lib.dir/codegen

CMakeFiles/test_lib.dir/src/test_lib.cpp.o: CMakeFiles/test_lib.dir/flags.make
CMakeFiles/test_lib.dir/src/test_lib.cpp.o: /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/src/test_lib.cpp
CMakeFiles/test_lib.dir/src/test_lib.cpp.o: CMakeFiles/test_lib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_lib.dir/src/test_lib.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/test_lib.dir/src/test_lib.cpp.o -MF CMakeFiles/test_lib.dir/src/test_lib.cpp.o.d -o CMakeFiles/test_lib.dir/src/test_lib.cpp.o -c /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/src/test_lib.cpp

CMakeFiles/test_lib.dir/src/test_lib.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/test_lib.dir/src/test_lib.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/src/test_lib.cpp > CMakeFiles/test_lib.dir/src/test_lib.cpp.i

CMakeFiles/test_lib.dir/src/test_lib.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/test_lib.dir/src/test_lib.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/src/test_lib.cpp -o CMakeFiles/test_lib.dir/src/test_lib.cpp.s

# Object files for target test_lib
test_lib_OBJECTS = \
"CMakeFiles/test_lib.dir/src/test_lib.cpp.o"

# External object files for target test_lib
test_lib_EXTERNAL_OBJECTS =

test_lib: CMakeFiles/test_lib.dir/src/test_lib.cpp.o
test_lib: CMakeFiles/test_lib.dir/build.make
test_lib: CMakeFiles/test_lib.dir/compiler_depend.ts
test_lib: lib/libdataProcessing.a
test_lib: /usr/lib/libgtest_main.so.1.15.2
test_lib: /usr/lib/libgtest.so.1.15.2
test_lib: CMakeFiles/test_lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_lib"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_lib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_lib.dir/build: test_lib
.PHONY : CMakeFiles/test_lib.dir/build

CMakeFiles/test_lib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_lib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_lib.dir/clean

CMakeFiles/test_lib.dir/depend:
	cd /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build /home/uditagrawal/Documents/researchIntern/branchone/DigitalTwin/tests/build/CMakeFiles/test_lib.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/test_lib.dir/depend

