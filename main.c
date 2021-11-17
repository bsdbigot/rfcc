#include "rfc822.h"
#include <zlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <ctype.h>

void usage( const char *pr )
{
	fprintf( stdout, "%s -i <in> -o <out> -l <log>\n", pr );
	exit( 1 );
}

void get_from_domain( const char *fromline, char **domain )
{
	int ct = 0;
	char *p = (char *)fromline;
	char *end = 0, *start = 0;
	while ( *p++ != '@' );
	start = p;
	while (( *p == '.' ) || ( isalnum( *p )) || ( *p == '-' ))
		p++;
	end = p;
	strncat( *domain, start, end-start );
}

int main( int argc, char **argv )
{
	FILE *err = 0;
	gzFile f = 0, out = 0;
	char ch = 0;
	char *buf = 0;
	char *logname = 0, *xmlname = 0, *datname = 0, *progname;
	char *bodyname = 0;
	char *campid = 0;
	char *domain = 0;
	char *record = 0;
	char *p;
	char *tok[10];
	unsigned long errct = 0;
	unsigned long lseq = 1;

	progname = basename( argv[0] );

	while( -1 != ( ch = getopt( argc, argv, "i:o:l:" )))
	{
		switch( ch )
		{
			case 'l':
				logname = optarg;
				break;
			case 'o':
				xmlname = optarg;
				break;
			case 'i':
				datname = optarg;
				break;
			default:
				usage( progname );
		}
	}
	argv += optind;
	argc -= optind;

	if (( !logname ) || ( !xmlname ) || ( !datname ))
		usage( progname );

	if ( 0 == ( err = fopen( logname, "w" ) ) )
		exit( 1 );
	if ( 0 == ( f = gzopen( datname, "r" ) ) )
		exit( 1 );
	if ( 0 == ( out = gzopen( xmlname, "w" ) ) )
		exit( 1 );
	if ( NULL == ( record = (char *)malloc(1024+1) ) )
		exit( 1 );
	if ( NULL == ( buf = (char *)malloc(4096+1) ) )
		exit( 1 );

	while( !gzeof( f ) )
	{
		if ( 0 == gzgets( f, record, 1024 ) )
			break;
		else if (( '\n' == *record ) || ( '\r' == *record ))
			break;
		errct = 0;
		for ( p = record; *p; p++ )
		{
			if (( *p == 10 ) || ( *p == 13 ))
			{
				*p = 0;
			}
			else if (( *p < 32 ) || ( *p > 126 ))
			{
				errct++;
			}
		}
		if ( errct > 0 )
		{
			fprintf( err, "%s\n", record );
			break;
		}

		if ( RFC822_VALID_FULL == Validate( record, strlen(record) ) )
		{
			gzprintf( out, "%s\n", record );
		}
		else
			fprintf( err, "%s\n", record );
		memset( record, 0, 1024 );
	}

	if ( record ) free ( record );
	if ( buf ) free ( buf );
	if ( domain ) free ( domain );

	gzclose( f );
	fclose( err );
	gzclose( out );

	return( 0 );
}
