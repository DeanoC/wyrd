cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(wyrd_libs)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)
FILE_GLOB_DIRS_ONLY(LEVELS .)

foreach(LEVEL ${LEVELS})
	add_subdirectory(${LEVEL})
endforeach()