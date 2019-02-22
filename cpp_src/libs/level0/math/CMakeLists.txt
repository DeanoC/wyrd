set( LibName math)

set( CInterface
		math.h
		)
set( CPPInterface
		)

set( Src
		)

set( Deps )

set(Tests
		test_math_c.cpp
		)

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}" "${Tests}")
