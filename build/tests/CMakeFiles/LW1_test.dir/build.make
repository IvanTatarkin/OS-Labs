# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_SOURCE_DIR = /home/ivan/OS-Labs

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ivan/OS-Labs/build

# Include any dependencies generated for this target.
include tests/CMakeFiles/LW1_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tests/CMakeFiles/LW1_test.dir/compiler_depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/LW1_test.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/LW1_test.dir/flags.make

tests/CMakeFiles/LW1_test.dir/lab1_test.cpp.o: tests/CMakeFiles/LW1_test.dir/flags.make
tests/CMakeFiles/LW1_test.dir/lab1_test.cpp.o: ../tests/lab1_test.cpp
tests/CMakeFiles/LW1_test.dir/lab1_test.cpp.o: tests/CMakeFiles/LW1_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ivan/OS-Labs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/LW1_test.dir/lab1_test.cpp.o"
	cd /home/ivan/OS-Labs/build/tests && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tests/CMakeFiles/LW1_test.dir/lab1_test.cpp.o -MF CMakeFiles/LW1_test.dir/lab1_test.cpp.o.d -o CMakeFiles/LW1_test.dir/lab1_test.cpp.o -c /home/ivan/OS-Labs/tests/lab1_test.cpp

tests/CMakeFiles/LW1_test.dir/lab1_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LW1_test.dir/lab1_test.cpp.i"
	cd /home/ivan/OS-Labs/build/tests && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ivan/OS-Labs/tests/lab1_test.cpp > CMakeFiles/LW1_test.dir/lab1_test.cpp.i

tests/CMakeFiles/LW1_test.dir/lab1_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LW1_test.dir/lab1_test.cpp.s"
	cd /home/ivan/OS-Labs/build/tests && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ivan/OS-Labs/tests/lab1_test.cpp -o CMakeFiles/LW1_test.dir/lab1_test.cpp.s

# Object files for target LW1_test
LW1_test_OBJECTS = \
"CMakeFiles/LW1_test.dir/lab1_test.cpp.o"

# External object files for target LW1_test
LW1_test_EXTERNAL_OBJECTS =

tests/LW1_test: tests/CMakeFiles/LW1_test.dir/lab1_test.cpp.o
tests/LW1_test: tests/CMakeFiles/LW1_test.dir/build.make
tests/LW1_test: LW1/libutils.a
tests/LW1_test: LW1/libparent.a
tests/LW1_test: LW1/libutils.a
tests/LW1_test: lib/libgtest_maind.a
tests/LW1_test: lib/libgtestd.a
tests/LW1_test: tests/CMakeFiles/LW1_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ivan/OS-Labs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable LW1_test"
	cd /home/ivan/OS-Labs/build/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LW1_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/LW1_test.dir/build: tests/LW1_test
.PHONY : tests/CMakeFiles/LW1_test.dir/build

tests/CMakeFiles/LW1_test.dir/clean:
	cd /home/ivan/OS-Labs/build/tests && $(CMAKE_COMMAND) -P CMakeFiles/LW1_test.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/LW1_test.dir/clean

tests/CMakeFiles/LW1_test.dir/depend:
	cd /home/ivan/OS-Labs/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ivan/OS-Labs /home/ivan/OS-Labs/tests /home/ivan/OS-Labs/build /home/ivan/OS-Labs/build/tests /home/ivan/OS-Labs/build/tests/CMakeFiles/LW1_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/LW1_test.dir/depend

