# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jh/15-445/15-445Learning/tmp/tmp.4D7u1QP09R

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jh/15-445/15-445Learning/tmp/tmp.4D7u1QP09R/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Pool_Allocator.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Pool_Allocator.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Pool_Allocator.dir/flags.make

CMakeFiles/Pool_Allocator.dir/main.cpp.o: CMakeFiles/Pool_Allocator.dir/flags.make
CMakeFiles/Pool_Allocator.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jh/15-445/15-445Learning/tmp/tmp.4D7u1QP09R/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Pool_Allocator.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Pool_Allocator.dir/main.cpp.o -c /home/jh/15-445/15-445Learning/tmp/tmp.4D7u1QP09R/main.cpp

CMakeFiles/Pool_Allocator.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Pool_Allocator.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jh/15-445/15-445Learning/tmp/tmp.4D7u1QP09R/main.cpp > CMakeFiles/Pool_Allocator.dir/main.cpp.i

CMakeFiles/Pool_Allocator.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Pool_Allocator.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jh/15-445/15-445Learning/tmp/tmp.4D7u1QP09R/main.cpp -o CMakeFiles/Pool_Allocator.dir/main.cpp.s

# Object files for target Pool_Allocator
Pool_Allocator_OBJECTS = \
"CMakeFiles/Pool_Allocator.dir/main.cpp.o"

# External object files for target Pool_Allocator
Pool_Allocator_EXTERNAL_OBJECTS =

Pool_Allocator: CMakeFiles/Pool_Allocator.dir/main.cpp.o
Pool_Allocator: CMakeFiles/Pool_Allocator.dir/build.make
Pool_Allocator: CMakeFiles/Pool_Allocator.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jh/15-445/15-445Learning/tmp/tmp.4D7u1QP09R/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Pool_Allocator"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Pool_Allocator.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Pool_Allocator.dir/build: Pool_Allocator

.PHONY : CMakeFiles/Pool_Allocator.dir/build

CMakeFiles/Pool_Allocator.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Pool_Allocator.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Pool_Allocator.dir/clean

CMakeFiles/Pool_Allocator.dir/depend:
	cd /home/jh/15-445/15-445Learning/tmp/tmp.4D7u1QP09R/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jh/15-445/15-445Learning/tmp/tmp.4D7u1QP09R /home/jh/15-445/15-445Learning/tmp/tmp.4D7u1QP09R /home/jh/15-445/15-445Learning/tmp/tmp.4D7u1QP09R/cmake-build-debug /home/jh/15-445/15-445Learning/tmp/tmp.4D7u1QP09R/cmake-build-debug /home/jh/15-445/15-445Learning/tmp/tmp.4D7u1QP09R/cmake-build-debug/CMakeFiles/Pool_Allocator.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Pool_Allocator.dir/depend
