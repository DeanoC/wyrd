set(LibName tinyformat)

set( Interface tinyformat.h)
set( CPPInterface
		)
set( Src tinyformat.cpp )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "")

target_compile_definitions(
		${LibName}
		INTERFACE
		TINYFORMAT_IMPLEMENTATION_FILE=0)