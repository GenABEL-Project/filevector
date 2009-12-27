/*
	This function should be re-implemented using Rprintf
	if compiled as a part of R library
*/

#include "frerror.h"


void error (const char * format, ... )
{
	va_list args;
	char buffer[256];
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
#ifdef R_R_H
	Rprintf("ERROR: %s",buffer);
#else
	printf("ERROR: %s",buffer);
//	perror(buffer);
#endif
	exit(EXIT_FAILURE);
}

void warning (const char * format, ... )
{
	va_list args;
	char buffer[256];
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
#ifdef R_R_H
	Rprintf("WARNING: %s",buffer);
#else
	printf("WARNING: %s",buffer);
#endif
}

void message (const char * format, ... )
{
	va_list args;
	char buffer[256];
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
#ifdef R_R_H
	Rprintf("%s",buffer);
#else
	printf("%s",buffer);
#endif
}
