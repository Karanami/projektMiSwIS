# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_SOURCE_DIR = /home/piotr/projects/linux_http_server_try1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/piotr/projects/linux_http_server_try1/out/Debug

# Include any dependencies generated for this target.
include CMakeFiles/http_tcp.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/http_tcp.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/http_tcp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/http_tcp.dir/flags.make

CMakeFiles/http_tcp.dir/main.cpp.o: CMakeFiles/http_tcp.dir/flags.make
CMakeFiles/http_tcp.dir/main.cpp.o: /home/piotr/projects/linux_http_server_try1/main.cpp
CMakeFiles/http_tcp.dir/main.cpp.o: CMakeFiles/http_tcp.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/piotr/projects/linux_http_server_try1/out/Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/http_tcp.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/http_tcp.dir/main.cpp.o -MF CMakeFiles/http_tcp.dir/main.cpp.o.d -o CMakeFiles/http_tcp.dir/main.cpp.o -c /home/piotr/projects/linux_http_server_try1/main.cpp

CMakeFiles/http_tcp.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/http_tcp.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/piotr/projects/linux_http_server_try1/main.cpp > CMakeFiles/http_tcp.dir/main.cpp.i

CMakeFiles/http_tcp.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/http_tcp.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/piotr/projects/linux_http_server_try1/main.cpp -o CMakeFiles/http_tcp.dir/main.cpp.s

CMakeFiles/http_tcp.dir/http_tcp_server.cpp.o: CMakeFiles/http_tcp.dir/flags.make
CMakeFiles/http_tcp.dir/http_tcp_server.cpp.o: /home/piotr/projects/linux_http_server_try1/http_tcp_server.cpp
CMakeFiles/http_tcp.dir/http_tcp_server.cpp.o: CMakeFiles/http_tcp.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/piotr/projects/linux_http_server_try1/out/Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/http_tcp.dir/http_tcp_server.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/http_tcp.dir/http_tcp_server.cpp.o -MF CMakeFiles/http_tcp.dir/http_tcp_server.cpp.o.d -o CMakeFiles/http_tcp.dir/http_tcp_server.cpp.o -c /home/piotr/projects/linux_http_server_try1/http_tcp_server.cpp

CMakeFiles/http_tcp.dir/http_tcp_server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/http_tcp.dir/http_tcp_server.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/piotr/projects/linux_http_server_try1/http_tcp_server.cpp > CMakeFiles/http_tcp.dir/http_tcp_server.cpp.i

CMakeFiles/http_tcp.dir/http_tcp_server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/http_tcp.dir/http_tcp_server.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/piotr/projects/linux_http_server_try1/http_tcp_server.cpp -o CMakeFiles/http_tcp.dir/http_tcp_server.cpp.s

# Object files for target http_tcp
http_tcp_OBJECTS = \
"CMakeFiles/http_tcp.dir/main.cpp.o" \
"CMakeFiles/http_tcp.dir/http_tcp_server.cpp.o"

# External object files for target http_tcp
http_tcp_EXTERNAL_OBJECTS =

http_tcp: CMakeFiles/http_tcp.dir/main.cpp.o
http_tcp: CMakeFiles/http_tcp.dir/http_tcp_server.cpp.o
http_tcp: CMakeFiles/http_tcp.dir/build.make
http_tcp: CMakeFiles/http_tcp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/piotr/projects/linux_http_server_try1/out/Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable http_tcp"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/http_tcp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/http_tcp.dir/build: http_tcp
.PHONY : CMakeFiles/http_tcp.dir/build

CMakeFiles/http_tcp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/http_tcp.dir/cmake_clean.cmake
.PHONY : CMakeFiles/http_tcp.dir/clean

CMakeFiles/http_tcp.dir/depend:
	cd /home/piotr/projects/linux_http_server_try1/out/Debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/piotr/projects/linux_http_server_try1 /home/piotr/projects/linux_http_server_try1 /home/piotr/projects/linux_http_server_try1/out/Debug /home/piotr/projects/linux_http_server_try1/out/Debug /home/piotr/projects/linux_http_server_try1/out/Debug/CMakeFiles/http_tcp.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/http_tcp.dir/depend

