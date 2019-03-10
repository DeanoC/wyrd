cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

FILE_GLOB_DIRS_ONLY(LIBS .)

foreach (LIB ${LIBS})
    add_subdirectory(${LIB})
endforeach ()