#ifndef __CONVERT_UTIL__
#define __CONVERT_UTIL__

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>

using namespace std;

#include "frversion.h"
#include "frerror.h"
#include "frutil.h"
#include "DatABELBaseCPP.h"
#include "filevector.h"


void text2fvf(
		string program_name, string infilename, string outfilename,
		string rownamesfilename, string colnamesfilename,
		int rownames, int colnames,
		int skiprows, int skipcols,
		int transpose, int Rmatrix,
		unsigned short int type
);


#endif
