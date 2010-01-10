#include "mergevars.h"

int main(int argc, char * argv[])
{
	print_mergevars_welcome();

	if (argc < 4) print_mergevars_usage(argv[0]);

	char * ifname1 = argv[1], * ifname2 = argv[2], * ofname = argv[3];
	unsigned long int cachesize = 64;
	if (argc>4) cachesize = atoi(argv[4]);
	if (cachesize < 0) error ("cache size must be positive long integer\n\n");
	std::cout << "Options in effect:\n";
	std::cout << "\tINFILE1   = " << ifname1 << "\n";
	std::cout << "\tINFILE2   = " << ifname2 << "\n";
	std::cout << "\tOUTFILE   = " << ofname << "\n";
	std::cout << "\tcachesize = " << cachesize << " Mb\n\n";

	filevector<float> indata1(ifname1, cachesize);
	filevector<float> indata2(ifname2, cachesize);

// sanity checks: can we merge these files?
// are dimensions compatible?
	if (indata1.data_type.nobservations != indata2.data_type.nobservations) 
		error("can not merge files with different number of observations\n\n");
	else
		message("file dimensions are compatible\n");
// are observation names the same?
	unsigned long int cmpfail = 0, i = 0;
	while (!cmpfail && i<indata1.data_type.nobservations) {
		cmpfail = strcmp(indata1.observation_names[i].name,indata2.observation_names[i].name);
		i++;
	}
	if (cmpfail) 
		warning("observation names are not the same in files '%s' and '%s', observation # %u, names are '%s' and '%s' (only first shown); name will be taken from the first file\n",ifname1,ifname2,i,indata1.observation_names[i-1].name,indata2.observation_names[i-1].name);
	else
		message("observation names are the same in files '%s' and '%s'\n\n",ifname1,ifname2);
	unsigned long int out_nvars = indata1.data_type.nvariables + indata2.data_type.nvariables;
	unsigned long int out_nobs = indata1.data_type.nobservations;
	std::cout << "initalizing FVF-file '" << ofname << "'...\n";
	initialize_empty_file(ofname, out_nvars, out_nobs, FLOAT);
	std::cout << "writing out the data ... \n";

	filevector<float> outdata(ofname, cachesize);

// copy observation names from the first object
	for (unsigned long int i=0;i<indata1.data_type.nobservations;i++) outdata.observation_names[i] = indata1.observation_names[i];

// copy var names and data
	float * tmpdat = new (std::nothrow) float [outdata.data_type.nobservations];
	if (!tmpdat) error("can not allocate memory for tmpdat\n\n");
	for (unsigned long int i=0;i<indata1.data_type.nvariables;i++)
	{
		outdata.variable_names[i] = indata1.variable_names[i];
		indata1.read_variable(i,tmpdat);
		outdata.write_variable(i,tmpdat);
	}
	for (unsigned long int i=indata1.data_type.nvariables;
	                  i<(indata1.data_type.nvariables+indata2.data_type.nvariables);
		    	  i++) 
	{
		outdata.variable_names[i] = indata2.variable_names[i-indata1.data_type.nvariables];
		indata2.read_variable(i-indata1.data_type.nvariables,tmpdat);
		outdata.write_variable(i,tmpdat);
	} 
	
	return(1);
}
