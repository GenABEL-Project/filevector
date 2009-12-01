#include <stdlib.h>
#include "frversion.h"
#include "text2fvf.h"
#include "mergevars.h"
#include "usage.h"

void print_text2fvf_usage(char * progname) 
{
	std::cout << "USAGE: " << progname << " -i INFILE -o OUTFILE [-s N1] [-r N2] [--colnames[=CFILE]] [-t]" << "\n";
	std::cout << "\n";
	std::cout << "\t--infile=INFILE    : the name of the input file containing matrix in text format\n";
	std::cout << "\t--outfile=OUTFILE  : name of the output file which will containing matrix in FVF format\n";
	std::cout << "\t--skipcols=N1      : skip N1 first columns when reading from the input text file\n";
	std::cout << "\t--rownames=N2      : row names are provided in the N2-th column of the text file\n";
	std::cout << "\t--colnames[=CFILE] : column names are provided either as the first line of the text\n";
	std::cout << "\t                     matrix text file (if no arguments supplied) or in a separate\n";
	std::cout << "\t                     text file CFILE\n";
	std::cout << "\t--transpose        : should the matrix be transposed\n";
	std::cout << "\n";
	std::cout << "The program will convert text matrix to filevector (FVF) format.\n";
	std::cout << "By default, the columns of the text matrix are considered as 'variables'\n";
	std::cout << "for which FVF will provide fast access. If you want it other way around,\n";
	std::cout << "please specify '-t' option.\n";
	std::cout << "\n";
	std::cout << "EXAMPLES: " << progname << " -i test.mldose -o test.mldose.fvf -s 2 -r 1\n\n";
	std::cout << "          Text matrix file contains two starting columns, first of which\n";
	std::cout << "          contains subjects IDs.\n";
	std::cout << "          Command will convert text matrix provided in 'test.mldose' to \n";
	std::cout << "          FVF-formated file 'test.mldose.fvf'. Columns will be stored as\n";
	std::cout << "          the fast-access FVF 'variables'\n";
	std::cout << "\n";
	std::cout << "          " << progname << " -i test.mldose -o test.mldose.fvf -s 2 -r 1 -t\n\n";
	std::cout << "          Same as above, but transposing the matrix. Rows will be stores as\n";
	std::cout << "          the fast-access FVF 'variables'\n";
	std::cout << "\n";
	std::cout << "          " << progname << " -i test.mldose -o test.mldose.fvf -s 2 -r 1 --colnames=test.snplist\n\n";
	std::cout << "          Same as example 1, but we tell that column names are stored in file\n";
	std::cout << "          'test.snplist' (mind the full format used: '--colnames=CFILE')\n";
	std::cout << "\n";
	exit(0);
}

void print_text2fvf_welcome() 
{
	std::cout << "\ntext2fvf v. " << T2F_VERSION << " (" << T2F_RELEASEDATE << ")\n"; 
	std::cout << "(C) " << T2F_AUTHORS << "\n\n";
	std::cout << "based on filevector v. " << FV_VERSION << " (" << FV_RELEASEDATE << ")\n";
	std::cout << "(C) " << FV_AUTHORS << "\n\n";
}



//


void print_mergevars_usage(char * progname) 
{
	std::cout << "USAGE: " << progname << " INFILE1 INFILE2 OUTFILE [cachesize]" << "\n";
	std::cout << "\n";
	std::cout << "\tINFILE1, INFILE2 : two files in FVF-format\n";
	std::cout << "\tOUTFILE          : name of file where merged data will be written to\n";
	std::cout << "\tcachesize        : size (Mb) of cache to be used (optional)\n";
	std::cout << "\n";
	exit(0);
}

void print_mergevars_welcome() 
{
	std::cout << "\nmergevars v. " << MERGEVARS_VERSION << " (" << MERGEVARS_RELEASEDATE << ")\n"; 
	std::cout << "(C) " << MERGEVARS_AUTHORS << "\n\n";
	std::cout << "based on filevector v. " << FV_VERSION << " (" << FV_RELEASEDATE << ")\n";
	std::cout << "(C) " << FV_AUTHORS << "\n\n";
}
