# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_COMMAND = /snap/clion/61/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/61/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/artem/Projects/C++/cpp_hw/Variant

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/artem/Projects/C++/cpp_hw/Variant/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Variant.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Variant.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Variant.dir/flags.make

CMakeFiles/Variant.dir/main.cpp.o: CMakeFiles/Variant.dir/flags.make
CMakeFiles/Variant.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/artem/Projects/C++/cpp_hw/Variant/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Variant.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Variant.dir/main.cpp.o -c /home/artem/Projects/C++/cpp_hw/Variant/main.cpp

CMakeFiles/Variant.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Variant.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/artem/Projects/C++/cpp_hw/Variant/main.cpp > CMakeFiles/Variant.dir/main.cpp.i

CMakeFiles/Variant.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Variant.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/artem/Projects/C++/cpp_hw/Variant/main.cpp -o CMakeFiles/Variant.dir/main.cpp.s

# Object files for target Variant
Variant_OBJECTS = \
"CMakeFiles/Variant.dir/main.cpp.o"

# External object files for target Variant
Variant_EXTERNAL_OBJECTS =

Variant: CMakeFiles/Variant.dir/main.cpp.o
Variant: CMakeFiles/Variant.dir/build.make
Variant: CMakeFiles/Variant.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/artem/Projects/C++/cpp_hw/Variant/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Variant"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Variant.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Variant.dir/build: Variant

.PHONY : CMakeFiles/Variant.dir/build

CMakeFiles/Variant.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Variant.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Variant.dir/clean

CMakeFiles/Variant.dir/depend:
	cd /home/artem/Projects/C++/cpp_hw/Variant/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/artem/Projects/C++/cpp_hw/Variant /home/artem/Projects/C++/cpp_hw/Variant /home/artem/Projects/C++/cpp_hw/Variant/cmake-build-debug /home/artem/Projects/C++/cpp_hw/Variant/cmake-build-debug /home/artem/Projects/C++/cpp_hw/Variant/cmake-build-debug/CMakeFiles/Variant.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Variant.dir/depend
