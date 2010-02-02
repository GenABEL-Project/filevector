#include "convert_util.h"

#define REPORT_EVERY 10000

void text2fvf(
		std::string program_name,
		std::string infilename, std::string outfilename,
		std::string rownamesfilename, std::string colnamesfilename,
		int rownames, int colnames,
		int skiprows, int skipcols,
		int transpose, int Rmatrix,
		unsigned short int type,
		bool quiet
)
{

	if (!quiet) message("Options in effect:\n");
	if (!quiet) message("\t --infile    = %s\n",infilename.c_str());
	if (!quiet) message("\t --outfile   = %s\n",outfilename.c_str());
	if (!quiet) message("\t --skiprows  = ");
	if (skiprows) if (!quiet) message("%d\n",skiprows); else if (!quiet) message("OFF\n");
	if (!quiet) message("\t --skipcols  = ");
	if (skipcols) if (!quiet) message("%d\n",skipcols); else if (!quiet) message("OFF\n");
	if (!quiet) message("\t --rncol     = ");
	if (rownames) if (!quiet) message("%d\n",rownames); else if (!quiet) message("OFF\n");
	if (!quiet) message("\t --cnrow     = ");
	if (colnames) if (!quiet) message("%d\n",colnames); else if (!quiet) message("OFF\n");

	if (colnamesfilename != "") colnames = -1;
	if (rownamesfilename != "") rownames = -1;
	if (!quiet) message("\t --colnames  = ");
	if (colnames) {
		if (colnamesfilename == "")
			if (!quiet) message("ON, using line %d of '%s'\n",colnames,infilename.c_str());
			else
				if (!quiet) message("ON, using data from file '%s'\n",colnamesfilename.c_str());
	} else if (!quiet) message("OFF\n");

	if (!quiet) message("\t --rownames  = ");
	if (rownames) {
		if (rownamesfilename == "")
			if (!quiet) message("ON, using column %d of '%s'\n",rownames,infilename.c_str());
			else
				if (!quiet) message("ON, using data from file '%s'\n",rownamesfilename.c_str());
	} else if (!quiet) message("OFF\n");

	if (!quiet) message("\t --transpose = ");
	if (transpose) if (!quiet) message("ON\n"); else if (!quiet) message("OFF\n");
	if (!quiet) message("\t --Rmatrix   = ");
	if (Rmatrix) if (!quiet) message("ON\n"); else if (!quiet) message("OFF\n");
	if (!quiet) message("\n");


	// check that it is mentioned how many columns to skip when reading row names
	if (rownamesfilename=="" && rownames && !skipcols)
		error("\n\nPlease tell how many columns to skip when you provide the column containing row names!\n\n");
	if (skipcols && (rownames > skipcols))
		error("rownames > skipcols");
	if (colnamesfilename=="" && colnames && !skiprows)
		error("\n\nPlease tell how many rows to skip when you provide the row containing column names!\n\n");
	if (skiprows && (colnames > skiprows))
		error("colnames > skiprows");


	std::ifstream infile(infilename.c_str());
	if (!infile) error("can not open file '%s' for reading\n\n",infilename.c_str());

	// column names specified in a separate file; check the number of columns
	unsigned long int words_in_colnamesfile = 0;
	if (colnames & colnamesfilename != "")
	{
		std::ifstream colnamesfile(colnamesfilename.c_str());
		std::string tmpstr;
		if (!colnamesfile) error("can not open column names file '%s'\n\n",colnamesfilename.c_str());
		while (colnamesfile >> tmpstr) words_in_colnamesfile++;
		colnamesfile.close();
		if (!quiet) message("number of names in column-names file '%s' is %d\n\n",colnamesfilename.c_str(),words_in_colnamesfile);
	}

	// row names specified in a separate file; check the number of columns
	unsigned long int words_in_rownamesfile = 0;
	if (rownames & rownamesfilename != "")
	{
		std::ifstream rownamesfile(rownamesfilename.c_str());
		std::string tmpstr;
		if (!rownamesfile) error("can not open row names file '%s'\n\n",rownamesfilename.c_str());
		while (rownamesfile >> tmpstr) words_in_rownamesfile++;
		rownamesfile.close();
		if (!quiet) message("number of names in row-names file '%s' is %d\n\n", rownamesfilename.c_str(), words_in_rownamesfile );
	}

	// find out the number of rows and columns + some checks
	if (!quiet) message("Finding the number of columns and rows in the file + integrity checks ... Line count:\n");
	unsigned long int infile_linecount = 0, line_wordcount = 0;
	long int last_word_count = -1;
	std::string sline, sword;
	while (getline(infile,sline))
	{
		infile_linecount++;
		if ((infile_linecount % REPORT_EVERY) == 0) {if (!quiet) message("\b\b\b\b\b\b\b\b\b\b\b\b%d",infile_linecount);std::cout.flush();}
		std::istringstream stream_sline(sline);
		line_wordcount = 0;
		while (stream_sline >> sword) line_wordcount++;
		//		if (firstline_add_one) {line_wordcount++;firstline_add_one=0;}
		if (last_word_count >=0 && last_word_count != line_wordcount)
			if (!(Rmatrix && (line_wordcount-1) == last_word_count))
				error("file '%s', line %u, number of elements is %u (previous line(s): %u)\n\n",
						infilename.c_str(),infile_linecount,line_wordcount,last_word_count);
		last_word_count = line_wordcount;
	}
	if (!quiet) message("\b\b\b\b\b\b\b\b\b\b\b\b%d",infile_linecount);std::cout.flush();
	if (!quiet) message("\n");
	infile.close();
	if (!quiet) message("file '%s' line count = %d\n",infilename.c_str(),infile_linecount);
	if (!quiet) message("file '%s' column count = %d\n",infilename.c_str(),line_wordcount);
	if ( infile_linecount == 0 || line_wordcount ==0 ) error("file '%s' contains no lines/columns\n\n",infilename.c_str());

	if (colnames & colnamesfilename != "")
		if ((line_wordcount - skipcols) != words_in_colnamesfile)
			error("number of column names (%lu) does not match number of data columns (%lu)\n\n",
					words_in_colnamesfile,(line_wordcount-skipcols));

	// read the data to memory

	unsigned long int ncols = line_wordcount - skipcols;
	unsigned long int nrows = infile_linecount - skiprows;

	unsigned long int num_data_elements = ncols * nrows;

	// if file is to be transposed we can read one line at a time and write it immediately
	if (transpose) num_data_elements = ncols;

	float * data = new (std::nothrow) float [num_data_elements];

	if (!data) error("can not get RAM for %u column and %u rows matrix\n\n",ncols,nrows);

	infile.open(infilename.c_str());
	if (!infile) error("can not open file '%s' for reading\n\n",infilename.c_str());

	// make empty filevector file
	unsigned long int out_nvars = 0, out_nobs = 0;
	if (transpose) {
		out_nvars = nrows;
		out_nobs = ncols;
	} else {
		out_nvars = ncols;
		out_nobs = nrows;
	}
	// HERE IS THE TYPE
	initialize_empty_file(outfilename, out_nvars, out_nobs, type, true);

	if (!quiet) message("number of variables in FVF-file '%s' will be %d\n",outfilename.c_str(),out_nvars);
	if (!quiet) message("number of observations in FVF-file '%s' will be %d\n\n",outfilename.c_str(),out_nobs);

	DatABELBaseCPP * outdata = new filevector(outfilename, (unsigned long int) 64); // this is not nice - fixed cache-size of 64 Mb

	fixedchar tmpname;

	// read column names
	if (colnames)
		if (colnamesfilename=="") {
			int cur_line = 0;
			while (cur_line<skiprows)
			{
				cur_line++;
				getline(infile,sline);
				std::istringstream stream_sline(sline);
				if (cur_line == colnames) {
					int toCol = ncols;
					if (!Rmatrix) toCol = ncols + skipcols;
					//					Rprintf("toCol=%u\n",toCol);
					for (unsigned long int i=0;i<toCol;i++) {
						stream_sline >> sword;
						if (Rmatrix || i>=skipcols) {
							int idx = i;
							if (!Rmatrix) idx = idx - skipcols;
							//							Rprintf("%u %u %s\n",i,idx,sword.c_str());
							strcpy(tmpname.name,sword.c_str());
							if (transpose)
								outdata->write_observation_name(idx,tmpname);
							else
								outdata->write_variable_name(idx,tmpname);
						}
					}
				}
			}
		} else {
			std::ifstream colnamesfile(colnamesfilename.c_str());
			if (!colnamesfile) error("can not open column names file '%s'\n\n",colnamesfilename.c_str());
			std::string tmpstr;
			for (unsigned long int i=0;i<ncols;i++) {
				colnamesfile >> tmpstr;
				strcpy(tmpname.name,tmpstr.c_str());
				if (transpose)
					outdata->write_observation_name(i,tmpname);
				else
					outdata->write_variable_name(i,tmpname);
			}
			colnamesfile.close();
		}

	// read row names file if present
	if (rownames && rownamesfilename != "")
	{
		std::ifstream rownamesfile(rownamesfilename.c_str());
		if (!rownamesfile) error("can not open row names file '%s'\n\n",rownamesfilename.c_str());
		std::string tmpstr;
		for (unsigned long int i=0;i<nrows;i++) {
			rownamesfile >> tmpstr;
			strcpy(tmpname.name,tmpstr.c_str());
			if (transpose)
				outdata->write_variable_name(i,tmpname);
			else
				outdata->write_observation_name(i,tmpname);
		}
		rownamesfile.close();
	}


	// read the data
	if (transpose)
		if (!quiet) message("reading data to RAM and writing to file '%s' ... Line count:\n",outfilename.c_str());
		else
			if (!quiet) message("reading data to RAM ... Line count:\n");
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
					outdata->write_variable_name(cline,tmpname);
				else
					outdata->write_observation_name(cline,tmpname);
			} else if (current_word > skipcols) {
				sscanf(sword.c_str(),"%f",&data[j++]);
			}
		}
		if (transpose) {
			outdata->write_variable_as(cline,data);
			j=0;
		}
		cline++;
		if ((cline % REPORT_EVERY) == 0) {if (!quiet) message("\b\b\b\b\b\b\b\b\b\b\b\b%d",cline);std::cout.flush();}
	}
	if (!quiet) message("\b\b\b\b\b\b\b\b\b\b\b\b%d",cline);std::cout.flush();
	if (!quiet) message("\n\n");
	infile.close();

	// check that row names are the same length as data
	if (rownames & rownamesfilename != "")
		if ((cline) != words_in_rownamesfile)
			error("number of row names (%lu) does not match number of data rows (%lu)\n\n",
					words_in_rownamesfile,(cline-skiprows));


	// we have to write data from memory...
	if (!transpose) {

		if (!quiet) message("Writing data to FVF-file '%s' ... Writing variable:\n",outfilename.c_str());

		float * tmpdat = new (std::nothrow) float [nrows];

		if (!tmpdat) error("can not get RAM for tmpdat\n\n");
		unsigned long int current_var;
		for (current_var=0;current_var<out_nvars;current_var++)
		{
			if (transpose)
			{
				outdata->write_variable_as(current_var,(data+current_var*ncols));
			} else {
				for (unsigned long int j=0;j<nrows;j++) tmpdat[j] = data[j * ncols + current_var];
				outdata->write_variable_as(current_var,tmpdat);
			}
			if ((current_var+1 % REPORT_EVERY) == 0) {if (!quiet) message("\b\b\b\b\b\b\b\b\b\b\b\b%d",current_var+1);std::cout.flush();}
		}
		if (!quiet) {
			message("\b\b\b\b\b\b\b\b\b\b\b\b%d",(current_var));
			std::cout.flush();
			message("\n\n");
		}

		delete [] tmpdat;

	}
	// from-memory block finished


	// free up the RAM
	delete [] data;
	delete outdata;
	if (!quiet) message("\nFinished successfully text2fvf\n");

}
