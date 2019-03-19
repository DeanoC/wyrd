cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(wyrd_apps)

FILE_GLOB_DIRS_ONLY(APPS .)

foreach(APP ${APPS})
	add_subdirectory(${APP})
endforeach()