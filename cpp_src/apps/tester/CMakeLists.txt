cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)

set(TESTER_SOURCE
		tester.cpp
		catch.hpp)

add_executable(tester ${TESTER_SOURCE})
target_link_libraries(tester
		core
		cmdlineshell
		lz4
		tinystl
		tinyformat
		singleheaders
		)
