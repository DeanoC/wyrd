#define CATCH_CONFIG_RUNNER
#include "core/core.h"
#include "catch.hpp"

#include "cmdlineshell/cmdlineshell.h"

int Main(int argc, char* argv[])
{
	return Catch::Session().run( argc, argv );

}

/*
TEST_CASE( "CityHash128 100 bytes", "[CityHash]" )
{
	using namespace CityHash;
	char smalldata[100];
	for(int i = 0; i < 100; ++i)
	{
		smalldata[i] = i;
	}

	uint128 result0 = CityHash::Hash128( smalldata, 100 );
	for(int i = 0; i < 100; ++i)
	{
		smalldata[i] = 1;
		uint128 result1 = CityHash::Hash128( smalldata, 100 );
		REQUIRE( result0.first != result1.first );
		REQUIRE( result0.second != result1.second );
	}
}

TEST_CASE( "CityHash128 1000 bytes", "[CityHash]" )
{
	using namespace CityHash;
	char data[1000];
	for(int i = 0; i < 1000; ++i)
	{
		data[i] = i;
	}

	uint128 result0 = CityHash::Hash128( data, 1000 );
	for(int i = 0; i < 1000; ++i)
	{
		data[i] = 1;
		uint128 result1 = CityHash::Hash128( data, 1000 );
		REQUIRE( result0.first != result1.first );
		REQUIRE( result0.second != result1.second );
	}
}
 */