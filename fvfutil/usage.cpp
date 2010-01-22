#include "usage.h"

using namespace std;

void print_text2fvf_usage(char * progname) 
{
	cout << "USAGE: " << progname << " -i INFILE -o OUTFILE [-r N2] [--colnames[=CFILE]] [-t] " << "\n";
	cout << "\n";
	cout << "\t--infile=INFILE    : the name of the input file containing matrix in text format\n";
	cout << "\t--outfile=OUTFILE  : name of the output file which will containing matrix in FVF format\n";
	cout << "\t--skipcols=N1      : skip N1 first columns when reading from the input text file\n";
	cout << "\t--rownames=N2      : row names are provided in the N2-th column of the text file\n";
	cout << "\t--colnames[=CFILE] : column names are provided either as the first line of the text\n";
	cout << "\t                     matrix text file (if no arguments supplied) or in a separate\n";
	cout << "\t                     text file CFILE\n";
	cout << "\t--transpose        : should the matrix be transposed\n";
	
	cout << "\n";
	cout << "The program will convert text matrix to filevector (FVF) format.\n";
	cout << "By default, the columns of the text matrix are considered as 'variables'\n";
	cout << "for which FVF will provide fast access. If you want it other way around,\n";
	cout << "please specify '-t' option.\n";
	cout << "\n";
	cout << "EXAMPLES: " << progname << " -i test.mldose -o test.mldose.fvf -s 2 -r 1\n\n";
	cout << "          Text matrix file contains two starting columns, first of which\n";
	cout << "          contains subjects IDs.\n";
	cout << "          Command will convert text matrix provided in 'test.mldose' to \n";
	cout << "          FVF-formated file 'test.mldose.fvf'. Columns will be stored as\n";
	cout << "          the fast-access FVF 'variables'\n";
	cout << "\n";
	cout << "          " << progname << " -i test.mldose -o test.mldose.fvf -s 2 -r 1 -t\n\n";
	cout << "          Same as above, but transposing the matrix. Rows will be stores as\n";
	cout << "          the fast-access FVF 'variables'\n";
	cout << "\n";
	cout << "          " << progname << " -i test.mldose -o test.mldose.fvf -s 2 -r 1 --colnames=test.snplist\n\n";
	cout << "          Same as example 1, but we tell that column names are stored in file\n";
	cout << "          'test.snplist' (mind the full format used: '--colnames=CFILE')\n";
	cout << "\n";
	exit(0);
}

void print_text2fvf_welcome() 
{
	cout << "\ntext2fvf v. " << T2F_VERSION << " (" << T2F_RELEASEDATE << ")\n"; 
	cout << "(C) " << T2F_AUTHORS << "\n\n";
	cout << "based on filevector v. " << FV_VERSION << " (" << FV_RELEASEDATE << ")\n";
	cout << "(C) " << FV_AUTHORS << "\n\n";
}
//
void print_mergevars_usage(char * progname)
{
	cout << "USAGE: " << progname << " INFILE1 INFILE2 OUTFILE [cachesize]" << "\n";
	cout << "\n";
	cout << "\tINFILE1, INFILE2 : two files in FVF-format\n";
	cout << "\tOUTFILE          : name of file where merged data will be written to\n";
	cout << "\tcachesize        : size (Mb) of cache to be used (optional)\n";
	cout << "\n";
	exit(0);
}

void print_mergevars_welcome() 
{
	cout << "\nmergevars v. " << MERGEVARS_VERSION << " (" << MERGEVARS_RELEASEDATE << ")\n"; 
	cout << "(C) " << MERGEVARS_AUTHORS << "\n\n";
	cout << "based on filevector v. " << FV_VERSION << " (" << FV_RELEASEDATE << ")\n";
	cout << "(C) " << FV_AUTHORS << "\n\n";
}
