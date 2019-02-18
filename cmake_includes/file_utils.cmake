MACRO(FILE_GLOB_DIRS_ONLY result dir)

	file(GLOB _all LIST_DIRECTORIES true RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${dir} CONFIGURE_DEPENDS * )
	file(GLOB _files LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${dir} * )

	set(${result} ${_all})

	list(LENGTH _files _filelen)

	if(_filelen)
		list(REMOVE_ITEM ${result} ${_files})
	endif()

ENDMACRO()

MACRO(ADD_LIB LibName CHeaders CPPHeaders Src Deps Tests)
	set(_cheaders ${CHeaders})
	set(_cppheaders ${CPPHeaders})
	list(TRANSFORM _cheaders PREPEND include/${LibName}/ )
	list(TRANSFORM _cppheaders PREPEND cppinclude/${LibName}/ )

	set(_deps ${Deps} )

	# first check
	set(_srcs ${Src})
	if(DEFINED _srcs)
		list(LENGTH _srcs _SrcLength)
	endif()

    set(_tests ${Tests})
    if (DEFINED _tests)
        list(LENGTH _tests _TestsLength)
    endif ()

    list(TRANSFORM _deps PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/../../)
    set(_depsc ${_deps})
    list(TRANSFORM _depsc APPEND /include)
    set(_depscpp ${_deps})
    list(TRANSFORM _depscpp APPEND /cppinclude)

    if(_SrcLength)
		list(TRANSFORM _srcs PREPEND src/ )
		add_library( ${LibName} ${_srcs} ${_cheaders} ${_cppheaders} )
		target_include_directories( ${LibName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
		target_include_directories( ${LibName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cppinclude )
		target_include_directories( ${LibName} PRIVATE ${_depsc} )
		target_include_directories( ${LibName} PRIVATE ${_depscpp} )
	else()
		add_library( ${LibName} INTERFACE )
		target_include_directories( ${LibName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include )
		target_include_directories( ${LibName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cppinclude )
		target_include_directories( ${LibName} INTERFACE ${_depsc} )
		target_include_directories( ${LibName} INTERFACE ${_depscpp} )
	endif()

    if (_TestsLength)
        list(TRANSFORM _tests PREPEND tests/)
        add_executable(${LibName}_tests ${_tests} ${_cheaders} ${_cppheaders})
        target_link_libraries(${LibName}_tests ${LibName} core cmdlineshell)
        target_include_directories(${LibName}_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)
        target_include_directories(${LibName}_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
        target_include_directories(${LibName}_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cppinclude)
        target_include_directories(${LibName}_tests PRIVATE ${_depsc} ${CMAKE_CURRENT_SOURCE_DIR}/../../level0/catch/include)
        target_include_directories(${LibName}_tests PRIVATE ${_depscpp} ${CMAKE_CURRENT_SOURCE_DIR}/../../level0/catch/cppinclude)

    endif ()


ENDMACRO()