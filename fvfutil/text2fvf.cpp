#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>

#include <getopt.h>

#include "frversion.h"
#include "frerror.h"
#include "frutil.h"
#include "DatABELBaseCPP.h"
#include "frvector.h"

#include "usage.h"
#include "text2fvf.h"

int main(int argc, char * argv[])
{

	print_text2fvf_welcome();

	int next_option;
	const char * const short_options = "i:o:s:r:c::t";
	const struct option long_options [] =
	{
		{"infile",    required_argument, NULL, 'i'},
		{"outfile",   required_argument, NULL, 'o'},
		{"skipcols",  required_argument, NULL, 's'},
		{"rownames",  required_argument, NULL, 'r'},
		{"colnames",  optional_argument, NULL, 'c'},
		{"transpose", no_argument,       NULL, 't'},
		{ NULL     ,  no_argument,       NULL,  0 }
	};

	char * program_name = argv[0];
	char * infilename = NULL, * outfilename = NULL, * colnamesfilename = NULL;
	long int rownames = 0, colnames = 0, transpose = 0, skipcols = 0;

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
			case 's':
				  skipcols = atoi(optarg);
				  break;
			case 'r':
				  rownames = atoi(optarg);
				  break;
			case 'c':
				  colnames = 1;
					colnamesfilename = optarg;
				  break;
			case 't':
				  transpose=1;
				  break;

			case '?': print_text2fvf_usage(program_name);
			case ':': print_text2fvf_usage(program_name);
			case -1 : break;
			default: abort();
		}
	}
	while (next_option != -1);

	// check that in- and out-filenames are supplied
	if (infilename == NULL || outfilename == NULL)
	{
		print_text2fvf_usage(program_name);
	}

	// check that it is mentioned how many columns to skip when reading row names
	if (rownames && !skipcols)
	{
		error("\n\nPlease tell how many columns to skip when you provide the column containing row names!\n\n");
	}


	std::cout << "Options in effect:" << "\n";
	std::cout << "\t --infile    = " << infilename << "\n";
	std::cout << "\t --outfile   = " << outfilename << "\n";
	std::cout << "\t --skipcols  = ";
	if (skipcols) std::cout << skipcols << "\n"; else std::cout << "OFF\n";
	std::cout << "\t --rownames  = ";
	if (rownames) std::cout << rownames << "\n"; else std::cout << "OFF\n";
	std::cout << "\t --colnames  = ";
	if (colnames) {
			if (colnamesfilename == NULL)
				std::cout << "ON, using first line of '" << infilename << "'\n";
			else
				std::cout << "ON, using data from file '" << colnamesfilename << "'\n";
	} else std::cout << "OFF\n";
	std::cout << "\t --transpose = ";
	if (transpose) std::cout << "ON\n"; else std::cout << "OFF\n";
	std::cout << "\n";

	std::ifstream infile(infilename);
	if (!infile) error("can not open file '%s' for reading\n\n",infilename);

	// column names specified in a separate file; check the number of columns
	unsigned long int words_in_colnamesfile = 0;
	if (colnames & colnamesfilename != NULL)
	{
			std::ifstream colnamesfile(colnamesfilename);
			std::string tmpstr;
			if (!colnamesfile) error("can not open column names file '%s'\n\n",colnamesfilename);
			while (colnamesfile >> tmpstr) words_in_colnamesfile++;
			colnamesfile.close();
			std::cout << "number of names in column-names file '" << colnamesfilename << "' is " << words_in_colnamesfile << "\n\n";
	}

// find out the number of rows and columns + some checks
	std::cout << "Finding the number of columns and rows in the file + integrity checks ... Line count:\n";
	unsigned long int infile_linecount = 0, line_wordcount = 0;
	long int last_word_count = -1;
	std::string sline, sword;
//	int firstline_add_one = 0;
//	if (rownames && colnames && colnamesfile!=NULL) firstline_add_one = 1;
	while (getline(infile,sline))
	{
		infile_linecount++;
		if ((infile_linecount % 100) == 0) {std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b" << infile_linecount;std::cout.flush();}
		std::istringstream stream_sline(sline);
		line_wordcount = 0;
		while (stream_sline >> sword) line_wordcount++;
//		if (firstline_add_one) {line_wordcount++;firstline_add_one=0;}
		if (last_word_count >=0 && last_word_count != line_wordcount)
			error("file '%s', line %u, number of elements is %u\n\n",infilename,infile_linecount,line_wordcount);
		last_word_count = line_wordcount;
	}
	std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b" << infile_linecount;std::cout.flush();
	std:: cout << "\n";
	infile.close();
	std::cout << "file '" << infilename << "' line count = " << infile_linecount << "\n";
	std::cout << "file '" << infilename << "' column count = " << line_wordcount << "\n\n";
	if ( infile_linecount == 0 || line_wordcount ==0 ) error("file '%s' contains no lines/columns\n\n",infilename);

	if (colnames & colnamesfilename != NULL)
	if ((line_wordcount - skipcols) != words_in_colnamesfile)
			error("number of column names (%lu) does not macth number of data columns (%lu)\n\n",words_in_colnamesfile,(line_wordcount-skipcols));

// read the data to memory

	unsigned long int ncols = line_wordcount - skipcols;
	unsigned long int nrows = infile_linecount;
	if (colnames && colnamesfilename==NULL) nrows--;

	unsigned long int num_data_elements = ncols * nrows;

// if file is to be transposed we can read one line at a time and write it immediately
	if (transpose) num_data_elements = ncols;

	float * data = new (std::nothrow) float [num_data_elements];

	if (!data) error("can not get RAM for %u column and %u rows matrix\n\n",ncols,nrows);

	infile.open(infilename);
	if (!infile) error("can not open file '%s' for reading\n\n",infilename);

// make empty filevector file
	unsigned long int out_nvars = 0;
	if (transpose) {
		out_nvars = nrows;
		initialize_empty_file(outfilename, out_nvars, ncols, FLOAT);
	} else {
		out_nvars = ncols;
		initialize_empty_file(outfilename, out_nvars, nrows, FLOAT);
	}

	std::cout << "number of variables in FVF-file '" << outfilename << "' will be " << out_nvars << "\n\n";

	filevector<float> outdata(outfilename, (unsigned long int) 64); // this is not nice - fixed cache-size of 16 Mb

	fixedchar tmpname;

	// read column names
	if (colnames)
	if (colnamesfilename==NULL) {
		getline(infile,sline);
		std::istringstream stream_sline(sline);
		for (unsigned long int i=0;i<ncols;i++) {
			stream_sline >> sword;
			strcpy(tmpname.name,sword.c_str());
			if (transpose)
				outdata.set_observation_name(i,tmpname);
			else
				outdata.set_variable_name(i,tmpname);
		}
	} else {
		std::ifstream colnamesfile(colnamesfilename);
		if (!colnamesfile) error("can not open column names file '%s'\n\n",colnamesfilename);
		std::string tmpstr;
		for (unsigned long int i=0;i<ncols;i++) {
			colnamesfile >> tmpstr;
			strcpy(tmpname.name,tmpstr.c_str());
			if (transpose)
				outdata.set_observation_name(i,tmpname);
			else
				outdata.set_variable_name(i,tmpname);
		}
		colnamesfile.close();
	}

	if (transpose)
		std::cout << "reading data to RAM and writing to file '" << outfilename << "' ... Line count:\n";
	else
		std::cout << "reading data to RAM ... Line count:\n";
	unsigned long int j = 0, cline = 0;
	while (getline(infile,sline))
	{
		std::istringstream stream_sline(sline);
		unsigned long int current_word = 0;
		while (stream_sline >> sword) {
				current_word++;
				if (current_word == rownames) {
					strcpy(tmpname.name,sword.c_str());
					if (transpose)
						outdata.set_variable_name(cline,tmpname);
					else
						outdata.set_observation_name(cline,tmpname);
				} else if (current_word > skipcols) {
					sscanf(sword.c_str(),"%f",&data[j++]);
				}
		}
		if (transpose) {
				outdata.write_variable(cline,data);
				j=0;
		}
		cline++;
		if ((cline % 100) == 0) {std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b" << cline;std::cout.flush();}
	}
	std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b" << cline;std::cout.flush();
	std::cout << "\n\n";
	infile.close();

// we have to wrte data from memory...
	if (!transpose) {

	std::cout << "Writing data to FVF-file '" << outfilename << "'... Writing variable:\n";

	float * tmpdat = new (std::nothrow) float [nrows];

	if (!tmpdat) error("can not get RAM for tmpdat\n\n");
	unsigned long int current_var;
	for (current_var=0;current_var<out_nvars;current_var++)
	{
		if (transpose)
		{
			outdata.write_variable(current_var,(data+current_var*ncols));
		} else {
			for (unsigned long int j=0;j<nrows;j++) tmpdat[j] = data[j * ncols + current_var];
			outdata.write_variable(current_var,tmpdat);
		}
		if ((current_var+1 % 1000) == 0) {std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b" << (current_var+1);std::cout.flush();}
	}
	std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b" << (current_var);std::cout.flush();
	std::cout << "\n\n";

	delete [] tmpdat;

	}
// from-memory block finished


// free up the RAM
	delete [] data;

	std::cout << "\nFinished successfully\n\n";

	return(0);
}
