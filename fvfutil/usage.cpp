#include "usage.h"

using namespace std;

void print_text2fvf_usage(char * progname)
{
    cout << "USAGE: " << progname << " -i INFILE -o OUTFILE [-r N2] "
         << "[--colnames[=CFILE]] [-t] [--datatype=TYPE]" << endl;
    cout << endl;
    cout << "\t-i INFILE    --infile=INFILE    : the name of the input file "
         << "containing matrix in text format" << endl;
    cout << "\t-o OUTFILE   --outfile=OUTFILE  : name of the output file which "
         << "will containing matrix in FVF format" << endl;
    cout << "\t-c N1        --skipcols=N1      : skip N1 first columns when reading "
         << "from the input text file" << endl;
    cout << "\t-r N2        --rownames=N2      : row names are provided in the N2-th "
         << "column of the text file" << endl;
    cout << "\t--colnames[=CFILE] : column names are provided either as "
         << "the first line of the text" << endl;
    cout << "\t                     matrix text file "
         << "(if no arguments supplied) or in a separate" << endl;
    cout << "\t                     text file CFILE" << endl;
    cout << "\t-t           --transpose        : should the matrix be transposed" << endl;
    cout << "\t-d TYPE      --datatype=TYPE    : specify destination data type, "
         << "default is DOUBLE" << endl;
    cout << "\t-n STRING    --nanString=STRING : specify the string used to code NAs" << endl;
    cout << "\t-R           --Rmatrix          : if this option is set, the "
         << "file format is assumed to follow" << endl;
    cout << "\t                                  the format of an R matrix "
         << " produced with the " << endl;
    cout << "\t                                  ‘write.table(...,col.names"
         << "=TRUE,row.names=TRUE)’ command" << endl;

    cout << endl;
    cout << "The program will convert text matrix to FileVector (FVF) format."
         << endl;
    cout << "By default, the columns of the text matrix are considered "
         << "as 'variables'" << endl;
    cout << "for which FVF will provide fast access. If you want it "
         << "the other way around," << endl;
    cout << "please specify the '-t' option." << endl;
    cout << "" << endl;
    cout << "EXAMPLES: " << progname << " -i test.mldose -o test.mldose.fvf "
         << "-c 2 -r 1" << endl;
    cout << "          Text matrix file contains two starting columns, "
         << "first of which" << endl;
    cout << "          contains the subject IDs." << endl;
    cout << "          The command will convert the text matrix provided "
         << "in 'test.mldose' to " << endl;
    cout << "          the FVF-formated file 'test.mldose.fvd and test.mldose.fvi'. "
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
         << "are stored in the file" << endl;
    cout << "          'test.snplist' (mind the full format "
         << "used: '--colnames=CFILE')" << endl;
    cout << "" << endl;
    exit(0);
}


void print_text2fvf_welcome()
{
    cout << "\ntext2fvf v" << T2F_VERSION
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
