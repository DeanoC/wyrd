cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(wyrd)

include (cmake_includes/file_utils.cmake)

add_subdirectory(libs)
add_subdirectory(apps)
