# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

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
CMAKE_SOURCE_DIR = /root/work/sanji/devel/6.WpasuplicnatLibrary

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/work/sanji/devel/6.WpasuplicnatLibrary

# Include any dependencies generated for this target.
include CMakeFiles/tos_wpa.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/tos_wpa.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tos_wpa.dir/flags.make

CMakeFiles/tos_wpa.dir/main.c.o: CMakeFiles/tos_wpa.dir/flags.make
CMakeFiles/tos_wpa.dir/main.c.o: main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/work/sanji/devel/6.WpasuplicnatLibrary/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/tos_wpa.dir/main.c.o"
	clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/tos_wpa.dir/main.c.o   -c /root/work/sanji/devel/6.WpasuplicnatLibrary/main.c

CMakeFiles/tos_wpa.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/tos_wpa.dir/main.c.i"
	clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/work/sanji/devel/6.WpasuplicnatLibrary/main.c > CMakeFiles/tos_wpa.dir/main.c.i

CMakeFiles/tos_wpa.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/tos_wpa.dir/main.c.s"
	clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/work/sanji/devel/6.WpasuplicnatLibrary/main.c -o CMakeFiles/tos_wpa.dir/main.c.s

CMakeFiles/tos_wpa.dir/main.c.o.requires:

.PHONY : CMakeFiles/tos_wpa.dir/main.c.o.requires

CMakeFiles/tos_wpa.dir/main.c.o.provides: CMakeFiles/tos_wpa.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/tos_wpa.dir/build.make CMakeFiles/tos_wpa.dir/main.c.o.provides.build
.PHONY : CMakeFiles/tos_wpa.dir/main.c.o.provides

CMakeFiles/tos_wpa.dir/main.c.o.provides.build: CMakeFiles/tos_wpa.dir/main.c.o


# Object files for target tos_wpa
tos_wpa_OBJECTS = \
"CMakeFiles/tos_wpa.dir/main.c.o"

# External object files for target tos_wpa
tos_wpa_EXTERNAL_OBJECTS =

tos_wpa: CMakeFiles/tos_wpa.dir/main.c.o
tos_wpa: CMakeFiles/tos_wpa.dir/build.make
tos_wpa: CMakeFiles/tos_wpa.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/work/sanji/devel/6.WpasuplicnatLibrary/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable tos_wpa"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tos_wpa.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tos_wpa.dir/build: tos_wpa

.PHONY : CMakeFiles/tos_wpa.dir/build

CMakeFiles/tos_wpa.dir/requires: CMakeFiles/tos_wpa.dir/main.c.o.requires

.PHONY : CMakeFiles/tos_wpa.dir/requires

CMakeFiles/tos_wpa.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tos_wpa.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tos_wpa.dir/clean

CMakeFiles/tos_wpa.dir/depend:
	cd /root/work/sanji/devel/6.WpasuplicnatLibrary && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/work/sanji/devel/6.WpasuplicnatLibrary /root/work/sanji/devel/6.WpasuplicnatLibrary /root/work/sanji/devel/6.WpasuplicnatLibrary /root/work/sanji/devel/6.WpasuplicnatLibrary /root/work/sanji/devel/6.WpasuplicnatLibrary/CMakeFiles/tos_wpa.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tos_wpa.dir/depend
