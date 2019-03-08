cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(wyrd)

include (cmake_includes/file_utils.cmake)

option(BUILD_TESTS "Build Test executubles" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/live)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/live)

add_subdirectory(src/libs)
add_subdirectory(src/apps)
