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
include LW1/CMakeFiles/lab1.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include LW1/CMakeFiles/lab1.dir/compiler_depend.make

# Include the progress variables for this target.
include LW1/CMakeFiles/lab1.dir/progress.make

# Include the compile flags for this target's objects.
include LW1/CMakeFiles/lab1.dir/flags.make

LW1/CMakeFiles/lab1.dir/main.c.o: LW1/CMakeFiles/lab1.dir/flags.make
LW1/CMakeFiles/lab1.dir/main.c.o: ../LW1/main.c
LW1/CMakeFiles/lab1.dir/main.c.o: LW1/CMakeFiles/lab1.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ivan/OS-Labs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object LW1/CMakeFiles/lab1.dir/main.c.o"
	cd /home/ivan/OS-Labs/build/LW1 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT LW1/CMakeFiles/lab1.dir/main.c.o -MF CMakeFiles/lab1.dir/main.c.o.d -o CMakeFiles/lab1.dir/main.c.o -c /home/ivan/OS-Labs/LW1/main.c

LW1/CMakeFiles/lab1.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lab1.dir/main.c.i"
	cd /home/ivan/OS-Labs/build/LW1 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ivan/OS-Labs/LW1/main.c > CMakeFiles/lab1.dir/main.c.i

LW1/CMakeFiles/lab1.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lab1.dir/main.c.s"
	cd /home/ivan/OS-Labs/build/LW1 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ivan/OS-Labs/LW1/main.c -o CMakeFiles/lab1.dir/main.c.s

# Object files for target lab1
lab1_OBJECTS = \
"CMakeFiles/lab1.dir/main.c.o"

# External object files for target lab1
lab1_EXTERNAL_OBJECTS =

LW1/lab1: LW1/CMakeFiles/lab1.dir/main.c.o
LW1/lab1: LW1/CMakeFiles/lab1.dir/build.make
LW1/lab1: LW1/libparent.a
LW1/lab1: LW1/libutils.a
LW1/lab1: LW1/CMakeFiles/lab1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ivan/OS-Labs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable lab1"
	cd /home/ivan/OS-Labs/build/LW1 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lab1.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
LW1/CMakeFiles/lab1.dir/build: LW1/lab1
.PHONY : LW1/CMakeFiles/lab1.dir/build

LW1/CMakeFiles/lab1.dir/clean:
	cd /home/ivan/OS-Labs/build/LW1 && $(CMAKE_COMMAND) -P CMakeFiles/lab1.dir/cmake_clean.cmake
.PHONY : LW1/CMakeFiles/lab1.dir/clean

LW1/CMakeFiles/lab1.dir/depend:
	cd /home/ivan/OS-Labs/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ivan/OS-Labs /home/ivan/OS-Labs/LW1 /home/ivan/OS-Labs/build /home/ivan/OS-Labs/build/LW1 /home/ivan/OS-Labs/build/LW1/CMakeFiles/lab1.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : LW1/CMakeFiles/lab1.dir/depend

