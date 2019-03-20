FILE_GLOB_DIRS_ONLY(LIBS .)

foreach (LIB ${LIBS})
    add_subdirectory(${LIB})
endforeach ()