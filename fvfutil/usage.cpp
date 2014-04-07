#include "usage.h"

using namespace std;

void print_text2fvf_usage(char * progname)
{
    cout << "USAGE: " << progname << " -i INFILE -o OUTFILE [-r N2] "
         << "[--colnames[=CFILE]] [-t] [-datatype=TYPE]" << endl;
    cout << endl;
    cout << "\t--infile=INFILE    : the name of the input file "
         << "containing matrix in text format" << endl;
    cout << "\t--outfile=OUTFILE  : name of the output file which "
         << "will containing matrix in FVF format" << endl;
    cout << "\t--skipcols=N1      : skip N1 first columns when reading "
         << "from the input text file" << endl;
    cout << "\t--rownames=N2      : row names are provided in the N2-th "
         << "column of the text file" << endl;
    cout << "\t--colnames[=CFILE] : column names are provided either as "
         << "the first line of the text" << endl;
    cout << "\t                     matrix text file "
         << "(if no arguments supplied) or in a separate" << endl;
    cout << "\t                     text file CFILE" << endl;
    cout << "\t--transpose        : should the matrix be transposed" << endl;
    cout << "\t--datatype=TYPE    : specify destination data type, "
         << "default is DOUBLE" << endl;

    cout << endl;
    cout << "The program will convert text matrix to FileVector (FVF) format."
         << endl;
    cout << "By default, the columns of the text matrix are considered "
         << "as 'variables'" << endl;
    cout << "for which FVF will provide fast access. If you want it "
         << "other way around," << endl;
    cout << "please specify '-t' option." << endl;
    cout << "" << endl;
    cout << "EXAMPLES: " << progname << " -i test.mldose -o test.mldose.fvf "
         << "-c 2 -r 1" << endl;
    cout << "          Text matrix file contains two starting columns, "
         << "first of which" << endl;
    cout << "          contains subjects IDs." << endl;
    cout << "          Command will convert text matrix provided "
         << "in 'test.mldose' to " << endl;
    cout << "          FVF-formated file 'test.mldose.fvf'. "
         << "Columns will be stored as" << endl;
    cout << "          the fast-access FVF 'variables'" << endl;
    cout << "" << endl;
    cout << "          " << progname << " -i test.mldose "
         << "-o test.mldose.fvf -c 2 -r 1 -t" << endl;
    cout << "          Same as above, but transposing the matrix. "
         << "Rows will be stores as" << endl;
    cout << "          the fast-access FVF 'variables'" << endl;
    cout << "" << endl;
    cout << "          " << progname << " -i test.mldose "
         << "-o test.mldose.fvf -c 2 -r 1 --colnames=test.snplist" << endl;
    cout << "          Same as example 1, but we tell that column names "
         << "are stored in file" << endl;
    cout << "          'test.snplist' (mind the full format "
         << "used: '--colnames=CFILE')" << endl;
    cout << "" << endl;
    exit(0);
}


void print_text2fvf_welcome()
{
    cout << "\ntext2fvf v. " << T2F_VERSION
         << " (" << T2F_RELEASEDATE
         << ")" << endl;
    cout << "(C) " << T2F_AUTHORS << endl;
    cout << "based on FileVector v. " << FV_VERSION
         << " (" << FV_RELEASEDATE
         << ")" << endl;
    cout << "(C) " << FV_AUTHORS << endl;
}


void print_mergevars_usage(char * progname)
{
    cout << "USAGE: " << progname
         << " INFILE1 INFILE2 OUTFILE [cachesize]" << endl;
    cout << "" << endl;
    cout << "\tINFILE1, INFILE2 : two files in FVF-format" << endl;
    cout << "\tOUTFILE          : name of file where merged data "
         << "will be written to" << endl;
    cout << "\tcachesize        : size (Mb) of cache to be used "
         << "(optional)" << endl;
    cout << "" << endl;
    exit(0);
}


void print_mergevars_welcome()
{
    cout << "\nmergevars v. " << MERGEVARS_VERSION
         << " (" << MERGEVARS_RELEASEDATE << ")" << endl;
    cout << "(C) " << MERGEVARS_AUTHORS << "\n" << endl;
    cout << "based on FileVector v. " << FV_VERSION
         << " (" << FV_RELEASEDATE << ")" << endl;
    cout << "(C) " << FV_AUTHORS << "\n" << endl;
}
