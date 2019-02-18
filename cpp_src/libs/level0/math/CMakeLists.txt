set( LibName math)

set( CInterface
		math.h
		)
set( CPPInterface
		)

set( Src
		)
set( Deps )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}" "")
