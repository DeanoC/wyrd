MACRO(FILE_GLOB_DIRS_ONLY result dir)

	file(GLOB _all LIST_DIRECTORIES true RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS * )
	file(GLOB _files LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} * )

	set(${result} ${_all})

	list(LENGTH _files _filelen)

	if(_filelen)
		list(REMOVE_ITEM ${result} ${_files})
	endif()

ENDMACRO()

MACRO(ADD_LIB LibName CHeaders CPPHeaders Src Deps)
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

	if(_SrcLength)
		list(TRANSFORM _srcs PREPEND src/ )
		add_library( ${LibName} ${_srcs} ${_cheaders} ${_cppheaders} )
		target_include_directories( ${LibName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
		target_include_directories( ${LibName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cppinclude )
		list(TRANSFORM _deps PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/../../)
		set(_depsc ${_deps})
		list(TRANSFORM _depsc APPEND /include)
		set(_depscpp ${_deps})
		list(TRANSFORM _depscpp APPEND /cppinclude)
		target_include_directories( ${LibName} PRIVATE ${_depsc} )
		target_include_directories( ${LibName} PRIVATE ${_depscpp} )
	else()
		add_library( ${LibName} INTERFACE )
		target_include_directories( ${LibName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include )
		target_include_directories( ${LibName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cppinclude )
		list(TRANSFORM _deps PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/../../)
		set(_depsc ${_deps})
		list(TRANSFORM _depsc APPEND /include)
		set(_depscpp ${_deps})
		list(TRANSFORM _depscpp APPEND /cppinclude)
		target_include_directories( ${LibName} INTERFACE ${_depsc} )
		target_include_directories( ${LibName} INTERFACE ${_depscpp} )
	endif()
ENDMACRO()