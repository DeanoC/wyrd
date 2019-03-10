/** \file exception.h
 * Exception Header.
 * provides macros to enable easy Exception handling
 * (c) 2000 Dean Calver
 */

#pragma once

#ifndef CORE_EXCEPTION_H
#define CORE_EXCEPTION_H

#include <string>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#if defined(USE_CPP_EXCEPTIONS)

#include <exception>

/// begin a try/catch block
#	define CoreTry			try
/// catch x type of exception 
#	define CoreCatch(x)	catch(x)
/// catch our Exception class of type x
#	define CoreCatchOurException(x)	catch( x ## _Exception const& err)
/// catch any and all exception
#	define CoreCatchAll	catch( ... )
/// catch any of our Exception class
#	define CoreCatchAllOurExceptions	catch( Except const& err)
/// catch any of std::exception class
#	define CoreCatchAllStdExceptions	catch( std::exception const& err)
/// this functions makes no throws
#	define CoreNoThrows	throw()
/// throw an x type of exception
#	define CoreThrow(x)	throw(x)

/// throw our exception class of type x and with some extra text "" for none
#	define CoreThrowException(x,ExtraText) throw x ## _Exception( ExtraText, __FILE__, __LINE__ )

/// pass this exception back up the catch chain
#	define CoreReThrow		throw

#else

/// begin a try/catch block
#	define CoreTry			
/// catch x type of exception 
#	define CoreCatch(x)	if( false )
/// catch our Exception class of type x
#	define CoreCatchOurException(x)	if( false )
/// catch any and all exception
#	define CoreCatchAll	if( false )
/// catch any of our Exception class
#	define CoreCatchAllOurExceptions	if( false )
/// catch any of std::exception class
#	define CoreCatchAllStdExceptions	if( false )
/// this functions makes no throws
#	define CoreNoThrows	
/// throw an x type of exception
#	define CoreThrow(x)	assert( false && #x )

/// throw our exception class of type x and with some extra text "" for none
#	define CoreThrowException(x,ExtraText) assert( false && #x && #ExtraText )

/// pass this exception back up the catch chain
#	define CoreReThrow		

#endif

/// Our Exception class, has Name, description, UserText, file and line exception occured at
#define DECLARE_EXCEPTION(Name, Description) 						\
class Name ## _Exception : public Except	 						\
{								                					\
public:																\
	Name ## _Exception ( const std::string& ExtraText, 				\
			const std::string& file, unsigned int line ) 			\
		: Except(ExtraText, file, line ){};							\
	const std::string getName( void ) const CoreNoThrows			\
		{ return std::string( #Name " Exception" ); };				\
	const std::string getDescription( void ) const CoreNoThrows		\
		{ return std::string( #Description ); }; 					\
}

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Enums and typedefs
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Classes and structures
//---------------------------------------------------------------------------

/**
Base exception class.
Exception inherits off this to have a uniform way of specifying exception data.
*/
class Except
#if defined( USE_CPP_EXCEPTIONS )
: std::exception 
#endif	     
{
public:
	Except()
		: UserText(),File(), Line(0){};
	Except(const std::string UserData, const std::string file, unsigned int line )
#if defined( USE_CPP_EXCEPTIONS )
     : std::exception(), UserText( UserData ), File(file), Line(line) 
#endif       
       {};

	virtual ~Except() CoreNoThrows {};
	//! text name of exception
	virtual const std::string getName() const CoreNoThrows { return std::string("Except"); };
	//! text description of exception
	virtual const std::string getDescription() const CoreNoThrows { return std::string("Unspecified exception occured"); };
	//! user text from description
	const std::string getUserText() const CoreNoThrows { return UserText; };
	//! File exception was thrown from
	const std::string getFile() const CoreNoThrows { return File; };
	//! Line exception was thrown from
	const unsigned int getLine() const CoreNoThrows { return Line; };
	
	// from std::exception
	virtual const char* what() const CoreNoThrows { return UserText.c_str(); }

private:
	std::string UserText;
	std::string File;
	unsigned int Line;

};


// standard errors
DECLARE_EXCEPTION(Objdelete, Object was deleted before all releases);
DECLARE_EXCEPTION(PtrError, The pointer was incorrect);

DECLARE_EXCEPTION(MemError, Unable to allocate any system memory);

DECLARE_EXCEPTION(FileNotFoundError, Unable to find the file);
DECLARE_EXCEPTION(FileError, Generic File error);

DECLARE_EXCEPTION(TodoError, TODO Not yet implemented);
DECLARE_EXCEPTION(ParamError, Some parameter was incorrect);
DECLARE_EXCEPTION(BoundCheckFail, A boundary check has failed);
DECLARE_EXCEPTION(AssertError, Assert Failed);
DECLARE_EXCEPTION(ProtocolError, Protocol is invalid);

INLINE void LogException( const Except& Ex ) {
	printf("%s(%i):%s %s %s\n",
		   Ex.getFile().c_str(),
		   Ex.getLine(),
		   Ex.getName().c_str(),
		   Ex.getDescription().c_str(),
		   Ex.getUserText().c_str() );
}

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Externals
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// End Header file
//---------------------------------------------------------------------------
#endif
