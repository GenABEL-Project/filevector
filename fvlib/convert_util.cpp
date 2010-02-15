#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "filevector.h"

using namespace std;

#include "frerror.h"

#include "convert_util.h"


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

