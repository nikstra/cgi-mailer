///////////////////////////////////////////////////////////////////////////////
//
//	FILE:
//		CGIQuery.cpp
//
//	DATE:
//		1999-03-14
//
//	DESCRIPTION:
//		Implementation file for CDIQurey class.
//
//	FUNCTIONS:
//		CGIQuery::Parse() throw ( CGI_Error )
//		CGIQuery::Decode()
//
//	PURPOSE:
//		Get and decode a URL encoded query string from a
//		HTML form.
//
//	NOTES:
//		Long names problem:
//		The compilation of programs using polyhedra may fail on
//		Solaris because the length of the symbols during linkage
//		becomes too big (which leads to error messages like
//		/usr/ccs/bin/as: \"/var/tmp/cca00218.s\", line 25376:
//		error: can't compute value of an expression involving an
//		external symbol).
//		A workaround for this problem is to compile with
//		-g
//		and to link with
//		-z nodefs -s
//
//		I have successfully compiled this on SunOS 5,6 with Gcc 2.8.0,
//		Windows NT4WS with MSVC 5, Windows 9x with Cygwin and
//		Linux RedHat 5.2 with Gcc 2.90.
//
//	AUTHOR:
//		Niklas Strand
//

#include <iostream>
#include <string>
#include <stdlib.h>
#include "CGIQuery.h"
#include "AutoPtr.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//
//	FUNCTION:	CGIQuery::Parse()
//
//	PURPOSE:	This function parses and decodes data sent from a HTML form.
//				The data is stored in a map. If an error occurs, a CGI_Error
//				exception is thrown.
//

void CGIQuery::Parse() throw ( CGI_Error )
{
	// Get environment variable, throw an exception
	// if not successful.
	RequestMethod = GetEnv( "REQUEST_METHOD" );
	if( !RequestMethod.length())
		throw CGI_Error( "Invlaid Request Method" );

	if( RequestMethod == "POST" )
	{
		// Get environment variable, throw an exception
		// if not successful.
		string strContentLength = GetEnv( "CONTENT_LENGTH" );
		if( !strContentLength.length())
			throw CGI_Error( "Invalid Content Length" );

		// Convert the length string to an int and allocate
		// a string with that length + 1, for the null char.
		int nContentLength = atoi( strContentLength.c_str());
		CAutoPtr<char> InBuffer( nContentLength + 1 );
		if( nContentLength > 0 )
		{
			// Read data, throw an exception if
			// something goes wrong.
			cin.read( InBuffer, nContentLength );
			if( !cin.good())
				throw CGI_Error( "Error Reading in POST" );
		}
		// Add a null char and save the string.
		InBuffer[nContentLength] = '\0';
		Text = InBuffer;
	}
	else
	{
		// Request method GET, read data form environment
		// variable, throw an exception if it isn't GET.
		// ( POST is already checked in the beginning of
		// the function ).
		if( RequestMethod == "GET" )
			Text = GetEnv( "QUERY_STRING" );
		else
			throw CGI_Error( "Unknown Request Method" );
	}

	size_t Beg;
	size_t End;
	size_t EqualSign;

	if( Text.find_first_of( "&=", 0 ) == string::npos )
		RequestMethod = "ISINDEX";
	else
	{
		for( Beg = 0, End = string::npos - 1; End != string::npos; Beg = End + 1 )
		{
			// Locate position of an & sign.
			End = Text.find_first_of( '&', Beg );

			// Locate position of an = sign.
			EqualSign = Text.find_first_of( '=', Beg );

			// Add it to the map, the string before the = sign as index
			// and the rest up to next & sign as data.
			DataMap[Decode( Text.substr( Beg, EqualSign - Beg ))]
				= Decode( Text.substr( EqualSign + 1, End - EqualSign - 1 ));
		}
	}
	Text = Decode( Text );
}

///////////////////////////////////////////////////////////////////////////////
//
//	FUNCTION:	CGIQuery::Decode()
//
//	PURPOSE:	This function decodes a URL encoded string and returns it.
//

string CGIQuery::Decode( const string &str ) const
{
	string strDecode = str;

	// Put the variables in processor registers.
	register unsigned int iGet;
	register unsigned int iSet;

	for( iGet = 0, iSet = 0; iGet < strDecode.length(); iGet++, iSet++ )
	{
		switch( strDecode[iGet] )
		{
		case '+': // Replace + sign with a space.
			strDecode[iSet] = ' ';
			break;
		case '%': // Convert the following two digits to ascii char.
			char HexCode[2];
			HexCode[0] = strDecode[iGet + 1];
			HexCode[1] = strDecode[iGet + 2];
			strDecode[iSet] = ( char )strtol( HexCode, NULL, 16 );
			iGet +=2;
			break;
		default: // Copy the char to new location.
			strDecode[iSet] = strDecode[iGet];
		}
	}
	strDecode.resize( iSet );

	return strDecode;
}
