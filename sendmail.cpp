///////////////////////////////////////////////////////////////////////////////
//
//	FILE:
//		sendmail.cpp
//
//	DATE:
//		1999-03-19
//
//	DESCRIPTION:
//		A CGI mailer, takes input from a HTML form and calls the sendmail
//		program through a pipe.
//
//	DECLARATIONS:
//
//	DEFINITIONS:
//		SENDMAIL
//		SENDMAIL_OPTS
//
//	FUNCTIONS:
//		main()
//
//	PURPOSE:
//
//	NOTES:
//
//	AUTHOR:
//		Niklas Strand
//

#include <fstream.h>	// Included for ofstream
#include <wait.h>		// Included for wait()
#include <unistd.h>		// Included for pipe(), fork()
#include <string>
#include <stdlib.h>
#include "CGIQuery.h"
#include "Pipe.h"

////////////////////////////////////////////////////////////
// SENDMAIL = The path to the sendmail program.
//
// SENDMAIL_OPTS = Command line options:
// -t  ignore mail adress on command line.
// -oi ignore lines beginning with a "."
//     in the message body.

#define SENDMAIL "/usr/lib/sendmail"
#define SENDMAIL_OPTS "-t", "-oi"

///////////////////////////////////////////////////////////
// A string containin all the reserved words, used as
// keys in the data map.
const string Reserved = "#to#from#subject#name#text#location#feedback#";
const string CONTENT_HTML = "Content-type: text/html\n";

///////////////////////////////////////////////////////////////////////////////
//
//	FUNCTION:	main()
//
//	PURPOSE:	The one and only main function.
//

int main()
{
	////////////////////////////////////////////////////////////
	// Create an instance of CGIQuery.
	CGIQuery Parser;

	////////////////////////////////////////////////////////////
	// A map to hold the form data.
	CGI_Data Data;

	try
	{
		////////////////////////////////////////////////////////////
		// Parse and decode the input from the form.
		Parser.Parse();

		////////////////////////////////////////////////////////////
		// Get the parsed data.
		Data = Parser.GetData();

		////////////////////////////////////////////////////////////
		// Verify that we are allowed to send mail to the
		// specified address.
		string address;
		ifstream MailAllow( "mail_allow", ios::in );
		if( !MailAllow.good())
			throw CGI_Error( "Couldn't verify \"To address\"" );

		bool allow = false;
		while( MailAllow.good())
		{
			MailAllow >> address;
			if( address == Data["to"] )
				allow = true;
		}
		MailAllow.close();
		if( !allow )
			throw CGI_Error( "Not allowed to send to this address!" );

		////////////////////////////////////////////////////////////
		// Check for users name and address, if empty write error
		// message to stdout.
		if( !Data["from"].length() || !Data["name"].length())
		{
			cout << CONTENT_HTML << endl
				<< "<HTML>\n<HEAD>\n<TITLE>NOT complete!</TITLE>\n</HEAD>\n"
				<< "<BODY>\n<H1>NOT complete!</H1><HR>\n<P>"
				<< "<B>You must enter both your name and E-mail address!</B>"
				<< "</P>\n<P><B><A HREF=\"JavaScript:history.back()\">"
				<< "Please, try again</A></B></P>\n</BODY>\n</HTML>" << endl;

			exit( EXIT_FAILURE );
		}

		////////////////////////////////////////////////////////////
		// CPipe holds a pair of file descriptors, fdPipe[0]
		// for reading and fdPipe[1] for writing.
		CPipe fdPipe;

		////////////////////////////////////////////////////////////
		// Create the filedescriptors, pipe() returns 0 on success
		// and -1 on failure. Throw an exception if unsuccessful.
		if( pipe( fdPipe ) != 0 )
			throw CGI_Error( "Could not open pipe" );

		////////////////////////////////////////////////////////////
		// Create a child process. fork() returns the process
		// PID or -1 on failure.
		pid_t pid;
		if(( pid = fork()) < 0 )
			throw CGI_Error( "Could not fork subprocess" );

		////////////////////////////////////////////////////////////
		// If fork() returned 0 we are the child process.
		if( pid )
		{
			////////////////////////////////////////////////////////////
			// We are the parent, close the read end of the pipe.
			close( fdPipe[READ_PIPE] );

			////////////////////////////////////////////////////////////
			// Create a stream connected to the pipe and write to it.
			ofstream SendMail( fdPipe[WRITE_PIPE] );

			////////////////////////////////////////////////////////////
			// Output the form data to the stream.
			SendMail << "To: " << Data["to"] << endl
				<< "From: " << Data["name"] << endl
				<< " <" << Data["from"] << ">" << endl
				<< "Subject: " << Data["subject"] << endl
				<< Data["text"] << endl;

			////////////////////////////////////////////////////////////
			// Output any additional data to the stream.
			SendMail << "\n*** Additional Parameters Sent ***" << endl;
			CGI_Data::iterator itr;
			for( itr = Data.begin(); itr != Data.end(); itr++ )
			{
				////////////////////////////////////////////////////////////
				// Don't output data from any reserved keys.
				if( Reserved.find(( *itr ).first ) == string::npos )
				{
					SendMail << ( *itr ).first << " = "
						<< ( *itr ).second << endl;
				}
			}

			////////////////////////////////////////////////////////////
			// Close the stream.
			SendMail.close();

			////////////////////////////////////////////////////////////
			// Wait until the child process exits.
			if( wait( 0 ) < 0 )
				throw CGI_Error( "Can't wait for my child" );

			////////////////////////////////////////////////////////////
			// Print any feedback to cout...
			if( Data["location"].length())
			{
				////////////////////////////////////////////////////////////
				// Redirects to another page.
				cout << "Location: " << Data["location"] << endl;
			}
			else
			{
				////////////////////////////////////////////////////////////
				// Prints your coustomized HTML data.
				if( Data["feedback"].length())
				{
					cout << CONTENT_HTML << endl;
					cout << Data["feedback"] << endl;
				}
			}

			////////////////////////////////////////////////////////////
			// Exit the parent process.
			exit( EXIT_SUCCESS );
		}
		else
		{
			////////////////////////////////////////////////////////////
			// We are the child, close the write end of the pipe.
			close( fdPipe[WRITE_PIPE] );

			////////////////////////////////////////////////////////////
			// Close stdin.
			close( STDIN_FILENO );

			////////////////////////////////////////////////////////////
			// Duplicate the file descriptor, redirect the pipe to cout.
			dup( fdPipe[READ_PIPE] );

			////////////////////////////////////////////////////////////
			// Close the read end of the pipe.
			close( fdPipe[READ_PIPE] );

			////////////////////////////////////////////////////////////
			// Execute the sendmail program. Not sure of the arguments??
			if( execl( SENDMAIL, SENDMAIL, SENDMAIL_OPTS, 0 ) < 0 )
				throw CGI_Error( "Can't execute sendmail" );
		}
	}
	catch( CGI_Error &e )
	{
		////////////////////////////////////////////////////////////
		// We caught an exception, print an error message in HTML
		// to cout.
		cout << CONTENT_HTML << endl
			<< "<HTML>\n<HEAD>\n<TITLE>CGI error!</TITLE>\n</HEAD>\n"
			<< "<BODY>\n<H1>CGI error!</H1><HR>\n<P>" << "Reported: <B>"
			<< e.what() << "</B></P>\n" << "<P>Please, contact the <A HREF="
			<< "\"mailto:nikstra@domain.tld?subject=cgi error\">webmaster</A>"
			<< "</P>\n</BODY>\n</HTML>" << endl;

		exit( EXIT_FAILURE );
	}

	return 0;
}
