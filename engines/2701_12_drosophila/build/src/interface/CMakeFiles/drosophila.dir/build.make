# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

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
CMAKE_COMMAND = /home/ors/.local/lib/python3.11/site-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /home/ors/.local/lib/python3.11/site-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ors/www/chess-continuum/dev/engines/2701_drosophila

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ors/www/chess-continuum/dev/engines/2701_drosophila/build

# Include any dependencies generated for this target.
include src/interface/CMakeFiles/drosophila.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/interface/CMakeFiles/drosophila.dir/compiler_depend.make

# Include the progress variables for this target.
include src/interface/CMakeFiles/drosophila.dir/progress.make

# Include the compile flags for this target's objects.
include src/interface/CMakeFiles/drosophila.dir/flags.make

src/interface/CMakeFiles/drosophila.dir/uci.c.o: src/interface/CMakeFiles/drosophila.dir/flags.make
src/interface/CMakeFiles/drosophila.dir/uci.c.o: /home/ors/www/chess-continuum/dev/engines/2701_drosophila/src/interface/uci.c
src/interface/CMakeFiles/drosophila.dir/uci.c.o: src/interface/CMakeFiles/drosophila.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ors/www/chess-continuum/dev/engines/2701_drosophila/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/interface/CMakeFiles/drosophila.dir/uci.c.o"
	cd /home/ors/www/chess-continuum/dev/engines/2701_drosophila/build/src/interface && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/interface/CMakeFiles/drosophila.dir/uci.c.o -MF CMakeFiles/drosophila.dir/uci.c.o.d -o CMakeFiles/drosophila.dir/uci.c.o -c /home/ors/www/chess-continuum/dev/engines/2701_drosophila/src/interface/uci.c

src/interface/CMakeFiles/drosophila.dir/uci.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/drosophila.dir/uci.c.i"
	cd /home/ors/www/chess-continuum/dev/engines/2701_drosophila/build/src/interface && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ors/www/chess-continuum/dev/engines/2701_drosophila/src/interface/uci.c > CMakeFiles/drosophila.dir/uci.c.i

src/interface/CMakeFiles/drosophila.dir/uci.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/drosophila.dir/uci.c.s"
	cd /home/ors/www/chess-continuum/dev/engines/2701_drosophila/build/src/interface && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ors/www/chess-continuum/dev/engines/2701_drosophila/src/interface/uci.c -o CMakeFiles/drosophila.dir/uci.c.s

# Object files for target drosophila
drosophila_OBJECTS = \
"CMakeFiles/drosophila.dir/uci.c.o"

# External object files for target drosophila
drosophila_EXTERNAL_OBJECTS =

drosophila: src/interface/CMakeFiles/drosophila.dir/uci.c.o
drosophila: src/interface/CMakeFiles/drosophila.dir/build.make
drosophila: libdrosophilaengine.a
drosophila: src/interface/CMakeFiles/drosophila.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ors/www/chess-continuum/dev/engines/2701_drosophila/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ../../drosophila"
	cd /home/ors/www/chess-continuum/dev/engines/2701_drosophila/build/src/interface && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/drosophila.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/interface/CMakeFiles/drosophila.dir/build: drosophila
.PHONY : src/interface/CMakeFiles/drosophila.dir/build

src/interface/CMakeFiles/drosophila.dir/clean:
	cd /home/ors/www/chess-continuum/dev/engines/2701_drosophila/build/src/interface && $(CMAKE_COMMAND) -P CMakeFiles/drosophila.dir/cmake_clean.cmake
.PHONY : src/interface/CMakeFiles/drosophila.dir/clean

src/interface/CMakeFiles/drosophila.dir/depend:
	cd /home/ors/www/chess-continuum/dev/engines/2701_drosophila/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ors/www/chess-continuum/dev/engines/2701_drosophila /home/ors/www/chess-continuum/dev/engines/2701_drosophila/src/interface /home/ors/www/chess-continuum/dev/engines/2701_drosophila/build /home/ors/www/chess-continuum/dev/engines/2701_drosophila/build/src/interface /home/ors/www/chess-continuum/dev/engines/2701_drosophila/build/src/interface/CMakeFiles/drosophila.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/interface/CMakeFiles/drosophila.dir/depend

