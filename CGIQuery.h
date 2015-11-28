///////////////////////////////////////////////////////////////////////////////
//
//	FILE:
//		CGIQuery.h
//
//	DATE:
//		1999-03-14
//
//	DESCRIPTION:
//		Declaration file for CGIQuery and CGI_Error classes.
//
//	DECLARATIONS:
//		CGI_Data
//
//	DEFINITIONS:
//		class CGI_Error
//		class CGIQuery
//
//	FUNCTIONS:
//		CGIQuery::GetData()
//		CGIQuery::GetEnv()
//		CGIQuery::GetText()
//		CGIQuery::GetRequestMethod()
//
//	PURPOSE:
//		See CGIQuery.cpp for purpose.
//
//	NOTES:
//		See CGIQuery.cpp for special notes.
//
//	AUTHOR:
//		Niklas Strand
//

#ifndef CGIQUERY_H
#define CGIQUERY_H

/////////////////////////////////////////////////
//	Disable the "identifier was truncated to
//	'255' characters in the debug information"
//	compiler warning.
#if defined( WIN32 ) && defined( _DEBUG )
#	pragma warning( disable : 4786 )
#	pragma message( "Compiler warnings: C4786 - Disabled!" )
#endif

#include <stdexcept>
#include <string>
#include <map>

using namespace std;

/////////////////////////////////////////////////
//	Define a STL map to store the decoded data.
// typedef less<string> order;
typedef map<string, string/*, order*/> CGI_Data;

///////////////////////////////////////////////////////////////////////////////
//
//	CLASS:		CGI_Error
//
//	PURPOSE:	Declaration of exception class.
//

class CGI_Error : public runtime_error
{
public:
	CGI_Error( const char *what_arg )
		: runtime_error( what_arg ) {}
};

///////////////////////////////////////////////////////////////////////////////
//
//	CLASS:		CGIQuery
//
//	PURPOSE:	Parse and decode the URL encoded data sent from a form.
//

class CGIQuery
{
public:
	CGI_Data GetData() const;
	string GetEnv( const char *var ) const;
	string GetText() const;
	string GetRequestMethod() const;
	void Parse() throw ( CGI_Error );

private:
	string Decode( const string &str ) const;

	CGI_Data DataMap;
	string RequestMethod;
	string Text;
};

///////////////////////////////////////////////////////////////////////////////
//
//	FUNCTION:	CGIQuery::GetData()
//
//	PURPOSE:	This function returns the parsed data in a map.
//

inline CGI_Data CGIQuery::GetData() const
{
	return DataMap;
}

///////////////////////////////////////////////////////////////////////////////
//
//	FUNCTION:	CGIQuery::GetEnv()
//
//	PURPOSE:	This function returns the value of the environment variable
//				given as argument. If the environment variable is invalid,
//				a zero length string is returned.
//

inline string CGIQuery::GetEnv( const char *var ) const
{
	char *EnvData = getenv( var );

	// The "false" string must have an empty ("")
	// string as argument to compile with MSVC.
	return EnvData ? string( EnvData ) : string( "" );
}

///////////////////////////////////////////////////////////////////////////////
//
//	FUNCTION:	CGIQuery::GetText()
//
//	PURPOSE:	This function returns the complete text.
//

inline string CGIQuery::GetText() const
{
	return Text;
}

///////////////////////////////////////////////////////////////////////////////
//
//	FUNCTION:	CGIQuery::GetRequestMethod()
//
//	PURPOSE:	This function returns which request metod that was used.
//

inline string CGIQuery::GetRequestMethod() const
{
	return RequestMethod;
}

#endif // CGIQUERY_H
