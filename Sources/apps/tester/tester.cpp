#define CATCH_CONFIG_RUNNER
#include "core/core.h"
#include "catch.hpp"

#include "cmdlineshell/cmdlineshell.h"

int Main(int argc, char* argv[])
{
	return Catch::Session().run( argc, argv );

}

/*
Shell::ShellInterface* globalShell;

int Main(Shell::ShellInterface& shell_)
{
	globalShell = &shell_; // just for test cases

	shell_.init({
		"Tester",
		true,
		false,
	});

	auto args = shell_.getArguments();
	std::vector<char const*> argv(args.size());
	for(auto i = 0u; i < args.size(); ++i)
	{
		argv[i] = args[i].c_str();
	}

	return Catch::Session().run( (int)args.size(), (char**)argv.data() );

}

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