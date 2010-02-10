#include <map>

using namespace std;

#include "text2fvf.h"

unsigned short int dataTypeFromString(string type){
    if (type == "UNSIGNED_SHORT_INT") return 1;
    if (type == "SHORT_INT") return 2;
    if (type == "UNSIGNED_INT") return 3;
    if (type == "INT") return 4;
    if (type == "FLOAT") return 5;
    if (type == "DOUBLE") return 6;
    return 0;
}

int main(int argc, char * argv[])
{
	print_text2fvf_welcome();

	int next_option;
	int quiet;
	const char * const short_options = "i:o:c:r:1:2:3:4:t:R:T:d";
	const struct option long_options [] =
	{
	    {"infile",    required_argument, NULL, 'i'},
		{"outfile",   required_argument, NULL, 'o'},
		{"skipcols",  required_argument, NULL, 'c'},
		{"skiprows",  required_argument, NULL, 'r'},
		{"rncol",     required_argument, NULL, '1'},
		{"cnrow",     required_argument, NULL, '2'},
		{"rnfile",    required_argument, NULL, '3'},
		{"cnfile",    required_argument, NULL, '4'},
		{"transpose", no_argument,       NULL, 't'},
		{"Rmatrix",   no_argument,       NULL, 'R'},
		{"outType",   required_argument, NULL, 'T'},
		{"dataType",  required_argument, NULL, 'd'},
		{ NULL     ,  no_argument,       NULL,  0 }
	};

	char *program_name = argv[0];
	char *infilename = NULL;
	char *outfilename = NULL;
	char *colnamesfilename = NULL;
	char *rownamesfilename = NULL;
	
	int rownames = 0, colnames = 0, transpose = 0, skipcols = 0, skiprows = 0, Rmatrix = 0;
    unsigned short dataType = 0;

	do
	{
		next_option = getopt_long(argc,argv,short_options,long_options,NULL);
		switch (next_option)
		{
		case 'i':
			infilename = optarg;
			break;
		case 'o':
			outfilename = optarg;
			break;
		case 'c':
			skipcols = atoi(optarg);
			break;
		case 'r':
			skiprows = atoi(optarg);
			break;
		case '1':
			rownames = atoi(optarg);
			break;
		case '2':
			colnames = atoi(optarg);
			break;
		case '3':
			rownames = 1;
			rownamesfilename = optarg;
			break;
		case '4':
			colnames = 1;
			colnamesfilename = optarg;
			break;
		case 't':
			transpose=1;
			break;
		case 'R':
			Rmatrix=1;
			break;
		case 'd':
		    dataType = dataTypeFromString(optarg);
		    break;

		case '?': print_text2fvf_usage(program_name);
		case ':': print_text2fvf_usage(program_name);
		case -1 : break;
		default: abort();
		}
	}
	while (next_option != -1);

    if (dataType == 0) {
        dataType = DOUBLE;
        message ("No output data type specified. Using DOUBLE.\n");
    }

	// check that in- and out-filenames are supplied
	if (infilename == NULL || outfilename == NULL)
	{
		print_text2fvf_usage(program_name);
	}

	if (colnamesfilename==NULL) colnamesfilename="";
	if (rownamesfilename==NULL) rownamesfilename="";
	
	string Pname = program_name, Iname = infilename, Oname = outfilename, RFname = rownamesfilename, CFname = colnamesfilename;

	text2fvf(Pname, Iname, Oname,
			RFname, CFname,
			rownames, colnames,
			skiprows, skipcols,
			transpose, Rmatrix, dataType, quiet);

	return(0);
}

#define REPORT_EVERY 10000

unsigned long calcNumLines(string fileName){
    ifstream file(fileName.c_str());
    string line;
    unsigned long numlines = 0;
    while(getline(file, line)){
        numlines++;
    }
    return numlines;
}

void parseStringToArbType(string s, int destType, void *destData) {
    map<int, string> fmt;

    fmt[UNSIGNED_SHORT_INT] = string("%hu");
    fmt[SHORT_INT] = string("%sd");
    fmt[UNSIGNED_INT] = string("%ud");
    fmt[INT] = string("%d");
    fmt[FLOAT] = string("%f");
    fmt[DOUBLE] = string("%lg");

    string format = fmt[destType];

    sscanf(s.c_str(), format.c_str(), destData);
}

void text2fvf(string program_name, string infilename, string outfilename,
	string rownamesfilename, string colnamesfilename, int rownames, int colnames,
	unsigned long skiprows, unsigned long skipcols, int transpose, int Rmatrix,
	unsigned short type, bool quiet) {

  	if (colnamesfilename != "") colnames = -1;
    if (rownamesfilename != "") rownames = -1;

    if (!quiet) {
        message("Options in effect: \n");
        message("\t --infile    = %s\n",infilename.c_str());
        message("\t --outfile   = %s\n",outfilename.c_str());
        message("\t --skiprows  = ");
    	if (skiprows)
    	    message("%d\n",skiprows);
	    else
	        message("OFF\n");

	    message("\t --skipcols  = ");
    	if (skipcols) message("%d\n",skipcols); else message("OFF\n");
    	message("\t --rncol     = ");
    	if (rownames) message("%d\n",rownames); else message("OFF\n");
    	message("\t --cnrow     = ");
    	if (colnames) message("%d\n",colnames); else message("OFF\n");
	        message("\t --colnames  = ");
    	if (colnames) {
	    	if (colnamesfilename == "")
		    	    message("ON, using line %d of '%s'\n",colnames,infilename.c_str());
			    else
			        message("ON, using data from file '%s'\n",colnamesfilename.c_str());
	    } else message("OFF\n");
	    message("\t --rownames  = ");

    	if (rownames) {
	    	if (rownamesfilename == "")
		    	message("ON, using column %d of '%s'\n",rownames,infilename.c_str());
			else
			    message("ON, using data from file '%s'\n",rownamesfilename.c_str());
	    } else
	        message("OFF\n");

	    message("\t --transpose = ");
	    messageOnOff(transpose);
	    message("\n");
    	message("\t --Rmatrix   = ");
    	messageOnOff(Rmatrix);
	    message("\n");
    }

	// check that it is mentioned how many columns to skip when reading row names
	if (rownamesfilename=="" && rownames && !skipcols)
		error("\n\nPlease tell how many columns to skip when you provide the column containing row names!\n\n");
	if (skipcols && (rownames > skipcols))
		error("rownames > skipcols");
	if (colnamesfilename=="" && colnames && !skiprows)
		error("\n\nPlease tell how many rows to skip when you provide the row containing column names!\n\n");
	if (skiprows && (colnames > skiprows))
		error("colnames > skiprows");

	ifstream infile(infilename.c_str());
	if (!infile) error("can not open file '%s' for reading\n\n",infilename.c_str());

	// column names specified in a separate file; check the number of columns
	unsigned long int words_in_colnamesfile = 0;
	if (colnames & colnamesfilename != "")
	{
		ifstream colnamesfile(colnamesfilename.c_str());
		string tmpstr;
		if (!colnamesfile) error("can not open column names file '%s'\n\n",colnamesfilename.c_str());
		while (colnamesfile >> tmpstr) words_in_colnamesfile++;
		colnamesfile.close();
		if (!quiet) message("number of names in column-names file '%s' is %d\n\n",colnamesfilename.c_str(),words_in_colnamesfile);
	}

	// row names specified in a separate file; check the number of columns
	unsigned long int words_in_rownamesfile = 0;
	if (rownames & rownamesfilename != "")
	{
		ifstream rownamesfile(rownamesfilename.c_str());
		string tmpstr;
		if (!rownamesfile) error("can not open row names file '%s'\n\n",rownamesfilename.c_str());
		while (rownamesfile >> tmpstr) words_in_rownamesfile++;
		rownamesfile.close();
		if (!quiet) message("number of names in row-names file '%s' is %d\n\n", rownamesfilename.c_str(), words_in_rownamesfile );
	}

    ifstream srcFile(infilename.c_str());

    string firstLine;

    getline(srcFile, firstLine);
    vector<string> firstLineWords;
    tokenize(firstLine, firstLineWords, " ");

    unsigned long numLines = calcNumLines(infilename);

    cout << "Number of lines in source file is " << numLines << endl;

    bool removeFirstElement = Rmatrix > 0;

    vector<string> resultColumns (firstLineWords.begin() + (removeFirstElement ? 1:0) + skipcols , firstLineWords.end());

    cout << "Number of columns is " << resultColumns.size() << endl;

    initialize_empty_file(outfilename, numLines - 1 - skiprows, resultColumns.size(), type, true );
    filevector out(outfilename, 1);

    string line;
    unsigned long rowCnt = 0;

    while(getline(srcFile, line)) {
        vector<string> lineWords;
        tokenize(line, lineWords, " ");

        string rowName = lineWords[0];

        unsigned long colCnt = 0;
        unsigned long i;
        for (i=1+colnames; i<lineWords.size(); i++){
            char buf [20];
            parseStringToArbType(lineWords[i], type, buf);
            out.write_element(rowCnt, colCnt, buf);
            colCnt++;
        }

        rowCnt++;
    }
}

