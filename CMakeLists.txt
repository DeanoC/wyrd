cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(wyrd)
set(CMAKE_MACOSX_RPATH 1)

include(FetchContent)
include (cmake_includes/file_utils.cmake)
include (cmake_includes/fetch_utils.cmake)

option(BUILD_TESTS "Build Test executubles" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/live)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/live)

set(LIB_BASE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/libs/)
set(APP_BASE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/apps/)

add_subdirectory(${LIB_BASE_PATH})
add_subdirectory(${APP_BASE_PATH})
