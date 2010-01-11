#ifndef __CONVERT_UTIL__
#define __CONVERT_UTIL__

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>

#include "frversion.h"
#include "frerror.h"
#include "frutil.h"
#include "DatABELBaseCPP.h"
#include "filevector.h"


void text2fvf(
		std::string program_name, std::string infilename, std::string outfilename,
		std::string rownamesfilename, std::string colnamesfilename,
		int rownames, int colnames,
		int skiprows, int skipcols,
		int transpose, int Rmatrix
);


#endif
