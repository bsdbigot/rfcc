#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "rfc822.h"

unsigned int atomize( const char *local, unsigned int len )
{
	//rfc[02]822 shows email address as localpart "@" domain
	//localpart consists of one or more words
	//  a dot (normally special) can appear between two words
	//word consists of one or more atoms
	//atom consists of any char excepting:
	//	     000 to 037 (octal)
	//	     127 or higher (decimal)
	//	     specials
	char *p, *q;
	char x822specials[] = "()<>@,;:\\\"[]";
	p = (char *)local;
	while( p < (local + len) )
	{
		if ( *p <= 037 )
			return( RFC822_INVALID );
		if ( *p >= 127 )
			return( RFC822_INVALID );
		if ( *p == '.' )
			if ( ( ! isalnum( *(p-1) ) ) || ( ! isalnum( *(p+1) ) ) )
				return( RFC822_INVALID );

		q = (char *)x822specials;
		while( *q )
		{
			if ( *p == *q )
				return( RFC822_INVALID );
			q++;
		}
		p++;
	}
	return( RFC822_VALID_FULL );
}

unsigned int domaincheck( const char *domain, unsigned int len )
{
	//rfc1034
	int hasdot = 0;
	char *p = (char *)domain;
	if ( len )
	{
		while( p < (domain + len ) )
		{
			if ( *p == '.' )
			{
				if ( ( ! isalnum( *(p-1) ) ) || ( ! isalnum( *(p+1) ) ) )
					return( RFC822_INVALID );
				hasdot = 1;
			}
			else
			{
				if (( ! isalnum( *p ) ) && ( *p != '-' ))
				 	return( RFC822_INVALID );
			}
			p++;
		}
	}
	else
		return( RFC822_VALID_LOCAL );

	if ( hasdot )
		return( RFC822_VALID_FULL );
	else
		return( RFC822_VALID_LOCAL );
}

void tokenize( const char *email, char **tok )
{
	char *p = (char *)email;
	while( isspace(*p++) ); //ignore leading white space
	tok[0] = p-1;
	while(( *p != '@' ) && ( *p ))
		p++;
	p++;
	tok[1] = p - email - 1;
	tok[2] = p;
	tok[3] = 0;
	while(*p)
	{
		if ( !isspace( *p++ ) )
			++tok[3];
		else
			break;
	}
}

int Validate( const char *email, unsigned int len )
{
	char *buf;
	char *tok[4];
	unsigned int status;

	if ( len )
	{
		if ( NULL == ( buf = (char *)malloc(len+1) ) )
			return( RFC822_MALLOC );
		memset( buf, 0, len+1 );
		strncpy( buf, email, len );
	}
	else
	{
		if ( NULL == ( buf = (char *)malloc(strlen(email)+1) ) )
			return( RFC822_MALLOC );
		memset( buf, 0, strlen(email)+1 );
		strcpy( buf, email );
	}

	tokenize( buf, tok );
	status = atomize( tok[0], (unsigned int)tok[1] );
	if ( RFC822_INVALID == status )
	{
		free( buf );
		return( RFC822_INVALID );
	}
	status = domaincheck( tok[2], (unsigned int)tok[3] );
	free( buf );
	return( status );
}
