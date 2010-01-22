#include "text2fvf.h"

int main(int argc, char * argv[])
{

	print_text2fvf_welcome();

	int next_option;
	const char * const short_options = "i:o:c:r:1:2:3:4:t:R";
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
			{ NULL     ,  no_argument,       NULL,  0 }
	};

	char * program_name = argv[0];
	char * infilename = NULL, * outfilename = NULL, * colnamesfilename = NULL, * rownamesfilename = NULL;
	int rownames = 0, colnames = 0, transpose = 0, skipcols = 0, skiprows = 0, Rmatrix = 0;

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

	if (colnamesfilename==NULL) colnamesfilename="";
	if (rownamesfilename==NULL) rownamesfilename="";
	std::string Pname = program_name, Iname = infilename, Oname = outfilename, RFname = rownamesfilename, CFname = colnamesfilename;
	text2fvf(Pname, Iname, Oname,
			RFname, CFname,
			rownames, colnames,
			skiprows, skipcols,
			transpose, Rmatrix,0);

	return(0);
}
