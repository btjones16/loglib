# Define the minumum CMake version. This version was chosen simply because it is
# the first version on which I tested the build. Feel free to try lowering it if
# your CMake version is older, chances are it will work.
cmake_minimum_required(VERSION 3.3)

# Define the project.
project(LogLib)

# Set the version info. The general syntax for releases is YYYY.MM.
set(LogLib_VERSION_MAJOR 2015)
set(LogLib_VERSION_MINOR 10)

# Create a single library from the loglib source code.
add_library(log src/log.c)

# The header files for loglib are in /include. Add this to the include path for
# both the loglib library and external projects that use it.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add compile time options.
option(RELEASE
       "Should messages with a severity level less than LOG_INFO be suppressed?"
       OFF)

option(DEBUG
       "Should LOG_ERROR and LOG_WARNING messages cause the program to exit with
       	error code EXIT_FAILURE immediately after printing?"
       OFF)

# Set the install locations.
install(TARGETS log DESTINATION lib)
install(FILES include/log.h DESTINATION include)

# Setup the testing.
add_executable(test_log EXCLUDE_FROM_ALL
			test/test_log.c test/testing_utilities.c
			test/cutest-1.5/cutest.c src/log.c)
enable_testing()
add_test(test_log test_log)
