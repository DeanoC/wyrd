MACRO(FILE_GLOB_DIRS_ONLY result dir)

	file(GLOB _all LIST_DIRECTORIES true RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS * )
	file(GLOB _files LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} * )

	set(${result} ${_all})

	list(LENGTH _files _filelen)

	if(_filelen)
		list(REMOVE_ITEM ${result} ${_files})
	endif()

ENDMACRO()

MACRO(ADD_LIB LibName Headers Src)
	list(TRANSFORM ${Headers} PREPEND ${LibName}/ )

	list(TRANSFORM ${Src} PREPEND impl/ )

	list(LENGTH ${Src} _SrcLength)

	if(_SrcLength)
		add_library( ${LibName} ${Src} ${Headers} )
		target_include_directories( ${LibName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
	else()
		add_library( ${LibName} INTERFACE )
		target_include_directories( ${LibName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
	endif()
ENDMACRO()