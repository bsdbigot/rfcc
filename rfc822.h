#ifndef HAVE_RFC822_H
#define HAVE_RFC822_H 1

#define RFC822_VALID_FULL	0
#define RFC822_VALID_LOCAL	1
#define RFC822_INVALID		2
#define RFC822_MALLOC		3

// Validate() requires a null terminated email address
int Validate( const char *email, unsigned int len );

#endif //HAVE_RFC822_H
