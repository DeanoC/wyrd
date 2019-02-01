cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(wyrd_libs)

FILE_GLOB_DIRS_ONLY(LEVELS .)

foreach(LEVEL ${LEVELS})
	add_subdirectory(${LEVEL})
endforeach()