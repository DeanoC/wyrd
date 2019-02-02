cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(wyrd_apps)

FILE_GLOB_DIRS_ONLY(APPS .)

foreach(APP ${APPS})
	file(GLOB _swift ${APP} *.swift )
	if(NOT _swift)
		add_subdirectory(${APP})
	endif()
endforeach()