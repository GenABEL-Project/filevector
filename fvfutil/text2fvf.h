#ifndef __TEXT2FVF_H__
#define __TEXT2FVF_H__

#include <getopt.h>

#include "usage.h"
#include "convert_util.h"

#define T2F_VERSION "0.0-2"
#define T2F_RELEASEDATE "July 28, 2009"
#define T2F_AUTHORS "Yurii Aulchenko"

void text2fvf(
    string program_name, string infilename, string outfilename, string rownamesfilename, string colnamesfilename,
	int rownames, int colnames,	unsigned long skiprows, unsigned long skipcols, int transpose, int Rmatrix, unsigned short int type, bool quiet
);


#endif
