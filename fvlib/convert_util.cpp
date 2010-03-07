#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <unistd.h>

using namespace std;

#include "filevector.h"
#include "Transposer.h"
#include "frerror.h"

#include "convert_util.h"

// old version (246) works with DatABEL

#define REPORT_EVERY 10000
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>

unsigned long calcNumLines(string fileName){
	ifstream file(fileName.c_str());
	string line;
	unsigned long numlines = 0;
	while(getline(file, line)){
		numlines++;
	}
	return numlines;
}

unsigned long calcNumWordsInFirstLine(string fileName){
	ifstream file(fileName.c_str());
	vector<string> words;
	string line;
	unsigned long numlines = 0;
	getline(file, line);
	tokenize(line, words);
	return words.size();
}

void text2fvf(string program_name, string infilename, string outfilename,
		string rownamesfilename, string colnamesfilename, int rncol, int cnrow,
		unsigned long skiprows, unsigned long skipcols, int bTranspose, int Rmatrix,
		unsigned short type, bool quiet) {

	/**
	rncol--;
	cnrow--;

	if (colnamesfilename != "") cnrow = 1;
	if (rownamesfilename != "") rncol = 1;
	 **/

	if (Rmatrix) {
		skipcols = skiprows = 1;
		rncol = cnrow = 1;
	}

	if (!quiet) {
		message("Options in effect: \n");
		message("\t --infile    = %s\n",infilename.c_str());
		message("\t --outfile   = %s\n",outfilename.c_str());

		message("\t --skiprows  = ");
		if (skiprows) message("%d\n",skiprows); else message("OFF\n");

		message("\t --skipcols  = ");
		if (skipcols) message("%d\n",skipcols); else message("OFF\n");

		/**
		message("\t --cnrow     = ");
		if (cnrow) message("%d\n",cnrow); else message("OFF\n");

		message("\t --rncol     = ");
		if (rncol) message("%d\n",rncol); else message("OFF\n");
		 **/

		message("\t --cnrow     = ");
		if (cnrow || colnamesfilename != "") {
			if (colnamesfilename == "")
				message("ON, using line %d of '%s'\n",cnrow,infilename.c_str());
			else
				message("ON, using data from file '%s'\n",colnamesfilename.c_str());
		} else {
			message("OFF\n");
		}

		message("\t --rncol     = ");

		if (rncol || rownamesfilename != "") {
			if (rownamesfilename == "")
				message("ON, using column %d of '%s'\n",rncol,infilename.c_str());
			else
				message("ON, using data from file '%s'\n",rownamesfilename.c_str());
		} else {
			message("OFF\n");
		}

		message("\t --transpose = ");
		messageOnOff(bTranspose);
		message("\n");
		message("\t --Rmatrix   = ");
		messageOnOff(Rmatrix);
		message("\n");
	}

	const string TMP_SUFFIX = "_fvtmp";
	string realOutFilename = outfilename;

	// if transpose if OFF, do transpose (sic) and write to temp file
	if (!bTranspose){
		outfilename = outfilename + TMP_SUFFIX;
	}

	// check that it is mentioned how many columns to skip when reading row names
	if (rownamesfilename=="" && (rncol>0) && !skipcols) {
		error("\n\nPlease tell how many columns to skip when you provide the column containing row names!\n\n");
	}
	if (skipcols && (rncol > skipcols)) {
		error("rncol > skipcols");
	}
	if (colnamesfilename=="" && (cnrow>0) && !skiprows) {
		error("\n\nPlease tell how many rows to skip when you provide the row containing column names!\n\n");
	}
	if (skiprows && (cnrow > skiprows)) {
		error("cnrow > skiprows");
	}

	ifstream infile(infilename.c_str());
	if (!infile) error("can not open file '%s' for reading\n\n",infilename.c_str());

	vector<string> extColNames;

	// column names specified in a separate file; check the number of columns
	unsigned long int words_in_colnamesfile = 0;

	// reading column names from file
	if (colnamesfilename != "")
	{
		cout << "Reading columns from "  << colnamesfilename << ": ";
		ifstream colnamesfile(colnamesfilename.c_str());
		string tmpstr;
		if (!colnamesfile) {
			error("can not open column names file '%s'\n\n", colnamesfilename.c_str());
		}
		while (colnamesfile >> tmpstr) {
			words_in_colnamesfile++;
			extColNames.push_back(tmpstr);
		}
		colnamesfile.close();
		if (!quiet) message("number of names in column-names file '%s' is %d\n\n",colnamesfilename.c_str(),words_in_colnamesfile);
	}


	vector<string> extRowNames;

	// row names specified in a separate file; check the number of columns
	unsigned long int words_in_rownamesfile = 0;

	// reading row names from file
	if (rownamesfilename != "") {
		ifstream rownamesfile(rownamesfilename.c_str());
		string tmpstr;
		if (!rownamesfile) {
			error("can not open row names file '%s'\n\n", rownamesfilename.c_str());
		}
		while (rownamesfile >> tmpstr) {
			words_in_rownamesfile++;
			extRowNames.push_back(tmpstr);
		}

		rownamesfile.close();

		if (!quiet)
			message("number of names in row-names file '%s' is %d\n\n", rownamesfilename.c_str(), words_in_rownamesfile );
	}

	ifstream srcFile(infilename.c_str());

	string firstLine;

	vector<string> firstLineWords;
	tokenize(firstLine, firstLineWords, " ");

	unsigned long numLines = calcNumLines(infilename);
	unsigned long numWords = calcNumWordsInFirstLine(infilename);

	cout << "Number of lines in source file is " << numLines << endl;
	cout << "Number of words in source file is " << numWords << endl;

	cout << "skiprows = " << skiprows << endl;
	cout << "cnrow = " << cnrow << endl;
	cout << "skipcols = " << skipcols << endl;
	cout << "rncol = " << rncol << endl;
	cout << "Rmatrix = " << Rmatrix << endl;
	cout << "numWords = " << numWords << endl;

	int numRows = numLines - skiprows;
	int numColumns=123;

	bool colNamesFilePresents = (colnamesfilename!="");
	bool rowNamesFilePresents = (rownamesfilename!="");

	/**
	if (Rmatrix) {
		if (colNamesFilePresents && rowNamesFilePresents){
			numColumns = numWords - skipcols;
		}
		if (colNamesFilePresents && !rowNamesFilePresents){
			numColumns = numWords - skipcols;
		}
		if (!colNamesFilePresents && rowNamesFilePresents){
			numColumns = numWords - skipcols;
		}
		if (!colNamesFilePresents && !rowNamesFilePresents){
			numColumns = numWords - skipcols + 1;//edited
		}
	} else {
		if (colNamesFilePresents&&rowNamesFilePresents){
			numColumns = numWords - skipcols; // edited
		}
		if (colNamesFilePresents&&!rowNamesFilePresents){
			numColumns = numWords - skipcols;//edited
		}
		if (!colNamesFilePresents&&rowNamesFilePresents){
			numColumns = numWords - skipcols;//edited
		}
		if (!colNamesFilePresents&&!rowNamesFilePresents){
			numColumns = numWords - skipcols;//edited
		}
	}
	 **/

	numColumns = numWords - skipcols;
	if (Rmatrix && !colNamesFilePresents)
		numColumns = numWords - skipcols + 1;

	cout << "Creating file with numRows = " << numRows << endl;
	cout << "Creating file with numColumns = " << numColumns << endl;
	initialize_empty_file(outfilename, numRows, numColumns, type, true );
	filevector *out = new filevector(outfilename, 1);

	string line;
	unsigned long rowCnt = 1;
	unsigned long lineCnt = 1;

	while(getline(srcFile, line)) {
		vector<string> lineWords;
		tokenize(line, lineWords, " ");

		// is this a column name line?
		if (lineCnt == cnrow && !colNamesFilePresents)
		{
			unsigned long i;
			// ignoring R-matrix flag for some reason
			for(i=skipcols-Rmatrix; i<lineWords.size(); i++) {
				extColNames.push_back(lineWords[i]);
			}
			lineCnt++;
			continue;
		}

		if (lineCnt <= skiprows)
		{
			lineCnt++;
			continue;
		}

		unsigned long colCnt = 0;
		unsigned long i;
		for (i=0; i<lineWords.size(); i++) {
			if (i == (rncol-1) && !rowNamesFilePresents) {
				extRowNames.push_back(lineWords[i]);
				continue;
			}

			if (i < skipcols) {
				continue;
			}

			char buf[20] = "01234567";
			parseStringToArbType(lineWords[i], type, buf);
			out->write_element(rowCnt-1, colCnt, buf);
			colCnt++;
		}

		rowCnt++;
		lineCnt++;
	}

	char * tmpstr;
	if (!colNamesFilePresents && cnrow < 0) for (unsigned long int i=1;i<=numColumns;i++) {
		sprintf(tmpstr,"%l",i);
		extColNames.push_back(tmpstr);
	}
	if (!rowNamesFilePresents && cnrow < 0) for (unsigned long int i=1;i<=numRows;i++) {
		sprintf(tmpstr,"%l",i);
		extRowNames.push_back(tmpstr);
	}
	unsigned long i;
	for (i=0;i<extColNames.size();i++){
		out->write_observation_name(i,extColNames[i]);
	}

	for (i=0;i<extRowNames.size();i++){
		out->write_variable_name(i,extRowNames[i]);
	}

	delete out;

	if(!bTranspose)
	{
		cout << "Transposing " << outfilename << " => " << realOutFilename << "." << endl;
		Transposer tr;

		tr.process(outfilename, realOutFilename, true);
	}
	cout << "text2fvf finished." << endl;
}

/**
void text2fvf(string program_name, string infilename, string outfilename,
	string rownamesfilename, string colnamesfilename, int rncol, int cnrow,
	unsigned long skiprows, unsigned long skipcols, int bTranspose, int Rmatrix,
	unsigned short type, bool quiet) {

    rncol--;
    cnrow--;

    if (colnamesfilename != "") cnrow = 1;
    if (rownamesfilename != "") rncol = 1;


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
    	if (rncol) message("%d\n",rncol); else message("OFF\n");
    	message("\t --cnrow     = ");
    	if (cnrow) message("%d\n",cnrow); else message("OFF\n");
	        message("\t --cnrow  = ");
    	if (cnrow) {
	    if (colnamesfilename == "")
	        message("ON, using line %d of '%s'\n",cnrow,infilename.c_str());
	    else
	        message("ON, using data from file '%s'\n",colnamesfilename.c_str());
	} else {
	    message("OFF\n");
	}

	message("\t --rncol  = ");

    	if (rncol) {
	    if (rownamesfilename == "")
		message("ON, using column %d of '%s'\n",rncol,infilename.c_str());
	    else
	        message("ON, using data from file '%s'\n",rownamesfilename.c_str());
	} else {
            message("OFF\n");
        }

        message("\t --transpose = ");
        messageOnOff(bTranspose);
        message("\n");
    	message("\t --Rmatrix   = ");
    	messageOnOff(Rmatrix);
        message("\n");
    }

    const string TMP_SUFFIX = "_fvtmp";
    string realOutFilename = outfilename;

    // if transpose if OFF, do transpose (sic) and write to temp file
    if (!bTranspose){
	outfilename = outfilename + TMP_SUFFIX;
    }

    // check that it is mentioned how many columns to skip when reading row names
    if (rownamesfilename=="" && rncol && !skipcols) {
    	error("\n\nPlease tell how many columns to skip when you provide the column containing row names!\n\n");
    }
    if (skipcols && (rncol > skipcols)) {
    	error("rncol > skipcols");
    }
    if (colnamesfilename=="" && cnrow && !skiprows) {
    	error("\n\nPlease tell how many rows to skip when you provide the row containing column names!\n\n");
    }
    if (skiprows && (cnrow > skiprows)) {
	error("cnrow > skiprows");
    }

    ifstream infile(infilename.c_str());
    if (!infile) error("can not open file '%s' for reading\n\n",infilename.c_str());

    vector<string> extColNames;

    // column names specified in a separate file; check the number of columns
    unsigned long int words_in_colnamesfile = 0;

    cout << "Reading columns from "  << colnamesfilename << ": ";
    if (colnamesfilename != "")
    {
	ifstream colnamesfile(colnamesfilename.c_str());
	string tmpstr;
	if (!colnamesfile) {
	    error("can not open column names file '%s'\n\n", colnamesfilename.c_str());
	}
	while (colnamesfile >> tmpstr) {
	    words_in_colnamesfile++;
	    extColNames.push_back(tmpstr);
	}
	colnamesfile.close();
	if (!quiet) message("number of names in column-names file '%s' is %d\n\n",colnamesfilename.c_str(),words_in_colnamesfile);
    }

    vector<string> extRowNames;

    // row names specified in a separate file; check the number of columns
    unsigned long int words_in_rownamesfile = 0;
    if (rownamesfilename != "") {
        ifstream rownamesfile(rownamesfilename.c_str());
        string tmpstr;
        if (!rownamesfile) {
	    error("can not open row names file '%s'\n\n", rownamesfilename.c_str());
	}
	while (rownamesfile >> tmpstr) {
	    words_in_rownamesfile++;
	    extRowNames.push_back(tmpstr);
	}

        rownamesfile.close();

	if (!quiet)
	     message("number of names in row-names file '%s' is %d\n\n", rownamesfilename.c_str(), words_in_rownamesfile );
    }

    ifstream srcFile(infilename.c_str());

    string firstLine;

    vector<string> firstLineWords;
    tokenize(firstLine, firstLineWords, " ");

    unsigned long numLines = calcNumLines(infilename);
    unsigned long numWords = calcNumWordsInFirstLine(infilename);

    cout << "Number of lines in source file is " << numLines << endl;
    cout << "Number of words in source file is " << numWords << endl;

    cout << "skiprows = " << skiprows << endl;
    cout << "cnrow = " << cnrow << endl;
    cout << "skipcols = " << skipcols << endl;
    cout << "rncol = " << rncol << endl;
    cout << "Rmatrix = " << Rmatrix << endl;
    cout << "numWords = " << numWords << endl;

    int numRows = numLines - skiprows;
    int numColumns=123;

    bool colNamesFilePresents = (colnamesfilename!="");
    bool rowNamesFilePresents = (rownamesfilename!="");

    if (Rmatrix) {
        if (colNamesFilePresents&&rowNamesFilePresents){
    	    numColumns = numWords - skipcols;
        }
        if (colNamesFilePresents&&!rowNamesFilePresents){
    	    numColumns = numWords - skipcols;
        }
        if (!colNamesFilePresents&&rowNamesFilePresents){
	    numColumns = numWords - skipcols;
	}
        if (!colNamesFilePresents&&!rowNamesFilePresents){
    	    numColumns = numWords - skipcols + 1;//edited
        }
    } else { 
        if (colNamesFilePresents&&rowNamesFilePresents){
    	    numColumns = numWords - skipcols; // edited
    	}
        if (colNamesFilePresents&&!rowNamesFilePresents){
    	    numColumns = numWords - skipcols;//edited
        }
        if (!colNamesFilePresents&&rowNamesFilePresents){
	    numColumns = numWords - skipcols;//edited
	}
        if (!colNamesFilePresents&&!rowNamesFilePresents){
    	    numColumns = numWords - skipcols;//edited
        }
    }

    cout << "Creating file with numRows = " << numRows << endl;
    cout << "Creating file with numColumns = " << numColumns << endl;
    initialize_empty_file(outfilename, numRows, numColumns, type, true );
    filevector *out = new filevector(outfilename, 1);

    string line;
    unsigned long rowCnt = 0;
    unsigned long lineCnt = 0;

    while(getline(srcFile, line)) {
        vector<string> lineWords;
        tokenize(line, lineWords, " ");

        // is this a column name line?
        if (lineCnt == cnrow && colnamesfilename == "") 
        {
    	    unsigned long i;
    	    // ignoring R-matrix flag for some reason
    	    for(i=skipcols-Rmatrix; i<lineWords.size(); i++) {
    		extColNames.push_back(lineWords[i]);
    	    }
    	    lineCnt++;
    	    continue;
    	}

	if (lineCnt < skiprows) 
	{
	    lineCnt++;
	    continue;
	}

        unsigned long colCnt = 0;
        unsigned long i;
        for (i=0; i<lineWords.size(); i++) {
    	    if (i == rncol && rownamesfilename == "") {
    		extRowNames.push_back(lineWords[i]);
    		continue;
    	    }

    	    if (i < skipcols) {
    		continue;
    	    }

            char buf[20] = "01234567";
            parseStringToArbType(lineWords[i], type, buf);
            out->write_element(rowCnt, colCnt, buf);
            colCnt++;
        }

        rowCnt++;
        lineCnt++;
    }

    unsigned long i;
    for (i=0;i<extColNames.size();i++){
        out->write_observation_name(i,extColNames[i]);
    }

    for (i=0;i<extRowNames.size();i++){
	out->write_variable_name(i,extRowNames[i]);
    }

    delete out;

    if(!bTranspose)
    {
	cout << "Transposing " << outfilename << " => " << realOutFilename << "." << endl;
	Transposer tr;

    	tr.process(outfilename, realOutFilename, true);
    }
    cout << "text2fvf finished." << endl;
}

 **/

