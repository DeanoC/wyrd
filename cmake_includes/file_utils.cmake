MACRO(FILE_GLOB_DIRS_ONLY result dir)

	file(GLOB _all LIST_DIRECTORIES true RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${dir} CONFIGURE_DEPENDS * )
	file(GLOB _files LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${dir} * )

	set(${result} ${_all})

	list(LENGTH _files _filelen)

	if(_filelen)
		list(REMOVE_ITEM ${result} ${_files})
	endif()

ENDMACRO()

if(APPLE)
	# We need to compile the interface builder *.xib files to *.nib files to add to the bundle
	# Make sure we can find the 'ibtool' program. If we can NOT find it we skip generation of this project
	FIND_PROGRAM( IBTOOL ibtool HINTS "/usr/bin" "${OSX_DEVELOPER_ROOT}/usr/bin" )
	if ( ${IBTOOL} STREQUAL "IBTOOL-NOTFOUND" )
		MESSAGE( SEND_ERROR "ibtool can not be found" )
	ENDIF()
endif()

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

    list(TRANSFORM _deps PREPEND ${LIB_BASE_PATH})
    set(_depsc ${_deps})
    list(TRANSFORM _depsc APPEND /include)
    set(_depscpp ${_deps})
    list(TRANSFORM _depscpp APPEND /cppinclude)

    if(_SrcLength)
		list(TRANSFORM _srcs PREPEND src/ )
		add_library( ${LibName} ${_srcs} ${_cheaders} ${_cppheaders} )
        foreach (_dep ${_deps})
            get_filename_component(deplibname ${_dep} NAME)
            target_link_libraries(${LibName} ${deplibname})
        endforeach ()
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
ENDMACRO()

MACRO(ADD_LIB_TESTS LibName CHeaders CPPHeaders Tests)
	set(_cheaders ${CHeaders})
	set(_cppheaders ${CPPHeaders})
	list(TRANSFORM _cheaders PREPEND include/${LibName}/ )
	list(TRANSFORM _cppheaders PREPEND cppinclude/${LibName}/ )

	set(_tests ${Tests})
	if (DEFINED _tests)
		list(LENGTH _tests _TestsLength)
	endif ()

	if (_TestsLength)
		list(TRANSFORM _tests PREPEND tests/)
		add_executable(${LibName}_tests ${_tests} ${_cheaders} ${_cppheaders})
		target_link_libraries(${LibName}_tests ${LibName} core cmdlineshell)
		target_include_directories(${LibName}_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)
		target_include_directories(${LibName}_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
		target_include_directories(${LibName}_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cppinclude)
		target_include_directories(${LibName}_tests PRIVATE ${_depsc} ${LIB_BASE_PATH}/level0/catch/include)
		target_include_directories(${LibName}_tests PRIVATE ${_depscpp} ${LIB_BASE_PATH}/level0/catch/cppinclude)
	endif ()
ENDMACRO()

MACRO(ADD_LIB_GUITESTS LibName CHeaders CPPHeaders Tests)
	set(_cheaders ${CHeaders})
	set(_cppheaders ${CPPHeaders})
	list(TRANSFORM _cheaders PREPEND include/${LibName}/ )
	list(TRANSFORM _cppheaders PREPEND cppinclude/${LibName}/ )

	set(_tests ${Tests})
	if (DEFINED _tests)
		list(LENGTH _tests _TestsLength)
	endif ()

	if (_TestsLength)
		list(TRANSFORM _tests PREPEND tests/)
		if(APPLE)
			list(APPEND _tests ${LIB_BASE_PATH}/level0/guishell/src/apple/appdelegate.h)
			list(APPEND _tests ${LIB_BASE_PATH}/level0/guishell/src/apple/appdelegate.m)
			list(APPEND _tests ${LIB_BASE_PATH}/level0/guishell/src/apple/macresources/MainMenu.nib)
		endif()

		add_executable(${LibName}_tests WIN32 MACOSX_BUNDLE ${_tests} ${_cheaders} ${_cppheaders})
		target_link_libraries(${LibName}_tests ${LibName} core guishell)
		target_include_directories(${LibName}_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)
		target_include_directories(${LibName}_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
		target_include_directories(${LibName}_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cppinclude)
		target_include_directories(${LibName}_tests PRIVATE ${_depsc} ${LIB_BASE_PATH}/level0/catch/include)
		target_include_directories(${LibName}_tests PRIVATE ${_depscpp} ${LIB_BASE_PATH}/level0/catch/cppinclude)

		if(APPLE)
			set_target_properties(${LibName}_tests PROPERTIES
					MACOSX_BUNDLE_GUI_IDENTIFIER com.wryd.${LibName}_tests
					MACOSX_BUNDLE_INFO_PLIST ${LIB_BASE_PATH}/level0/guishell/src/apple/macresources/Info.plist.in
					RESOURCE ${LIB_BASE_PATH}/level0/guishell/src/apple/macresources/MainMenu.nib
					)
		endif()
	endif ()
ENDMACRO()

MACRO(ADD_CONSOLE_APP AppName Src Deps)
	set(_src ${Src})
	set(_deps ${Deps} )
	add_executable(${AppName} ${_src})
	target_link_libraries(${AppName} ${LibName} core cmdlineshell)
	foreach (_dep ${_deps})
		get_filename_component(deplibname ${_dep} NAME)
		target_link_libraries(${AppName} ${deplibname})
	endforeach ()
ENDMACRO()

MACRO(ADD_GUI_APP AppName Src Deps)
	set(_src ${Src})
	set(_deps ${Deps} )

	if(APPLE)
		list(APPEND _src ${LIB_BASE_PATH}/level0/guishell/src/apple/appdelegate.h)
		list(APPEND _src ${LIB_BASE_PATH}/level0/guishell/src/apple/appdelegate.m)
		list(APPEND _src ${LIB_BASE_PATH}/level0/guishell/src/apple/macresources/MainMenu.nib)
	endif()

	add_executable(${AppName}  WIN32 MACOSX_BUNDLE ${_src})
	target_link_libraries(${AppName} ${LibName} core guishell)
	foreach (_dep ${_deps})
		get_filename_component(deplibname ${_dep} NAME)
		target_link_libraries(${AppName} ${deplibname})
	endforeach ()
	if(APPLE)
		set_target_properties(${AppName} PROPERTIES
				MACOSX_BUNDLE_GUI_IDENTIFIER com.wryd.${AppName}
				MACOSX_BUNDLE_INFO_PLIST ${LIB_BASE_PATH}/level0/guishell/src/apple/macresources/Info.plist.in
				RESOURCE ${LIB_BASE_PATH}/level0/guishell/src/apple/macresources/MainMenu.nib
				)
	endif()

ENDMACRO()
