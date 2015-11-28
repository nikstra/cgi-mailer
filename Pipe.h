///////////////////////////////////////////////////////////////////////////////
//
//	FILE:
//		Pipe.h
//
//	DATE:
//		1999-03-19
//
//	DESCRIPTION:
//		A wrapper for the pipe function, closes the pipes on destruction.
//
//	DECLARATIONS:
//		class CPipe
//
//	DEFINITIONS:
//		READ_PIPE
//		WRITE_PIPE
//
//	FUNCTIONS:
//		CPipe::CPipe()
//		CPipe::~CPipe()
//		operator int* ()
//
//	PURPOSE:
//
//	NOTES:
//
//	AUTHOR:
//		Niklas Strand
//

#ifndef PIPE_H
#define PIPE_H

#include <unistd.h>

#define READ_PIPE 0
#define WRITE_PIPE 1

///////////////////////////////////////////////////////////////////////////////
//
//	CLASS:		CPipe
//
//	PURPOSE:	Opens a pipe and closes it on destruction.
//

class CPipe
{
public:
	CPipe();
	~CPipe();
	operator int * ();

private:
	int PipeDescr[2];
};

///////////////////////////////////////////////////////////////////////////////
//
//	FUNCTION:	CPipe::CPipe()
//
//	PURPOSE:	Constructor, opens a pipe...
//

inline CPipe::CPipe()
{
	if( pipe( PipeDescr ) != 0 )
		throw CGI_Error( "Could not open pipe" );
}

///////////////////////////////////////////////////////////////////////////////
//
//	FUNCTION:	CPipe::~CPipe()
//
//	PURPOSE:	Destructor, closes the pipe...
//

inline CPipe::~CPipe()
{
	close( PipeDescr[0] );
	close( PipeDescr[1] );
}

///////////////////////////////////////////////////////////////////////////////
//
//	FUNCTION:	CPipe::operator int * ()
//
//	PURPOSE:	Returns a pointer to the file descriptor array.
//

inline CPipe::operator int * ()
{
	return PipeDescr;
}

#endif // PIPE_H