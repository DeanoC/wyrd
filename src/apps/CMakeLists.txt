FILE_GLOB_DIRS_ONLY(APPS .)

foreach(APP ${APPS})
	add_subdirectory(${APP})
endforeach()