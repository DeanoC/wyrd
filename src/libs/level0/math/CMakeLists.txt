set( LibName math)

set( CInterface
		math.h
		)
set( CPPInterface
		math.hpp
		)

set( Src
		math.cpp
		)

set(Deps
		level0/core
		)

set(Tests
		test_math_c.cpp
		)

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
ADD_LIB_TESTS(${LibName} "${CInterface}" "${CPPInterface}" "${Tests}")
