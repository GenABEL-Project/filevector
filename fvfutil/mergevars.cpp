#include "mergevars.h"

int main(int argc, char * argv[])
{
	print_mergevars_welcome();

	if (argc < 4) print_mergevars_usage(argv[0]);

	char * ifname1 = argv[1], * ifname2 = argv[2], * ofname = argv[3];
	unsigned long int cachesize = 64;
	if (argc>4) cachesize = atoi(argv[4]);
	if (cachesize < 0) err << "cache size must be positive long integer" << endl << endl << errorExit;
	dbg << "Options in effect:\n";
	dbg << "\tINFILE1   = " << ifname1 << "\n";
	dbg << "\tINFILE2   = " << ifname2 << "\n";
	dbg << "\tOUTFILE   = " << ofname << "\n";
	dbg << "\tcachesize = " << cachesize << " Mb\n\n";

	filevector indata1(ifname1, cachesize);
	filevector indata2(ifname2, cachesize);

// sanity checks: can we merge these files?
// are dimensions compatible?
	if (indata1.data_type.nobservations != indata2.data_type.nobservations)
		err << "can not merge files with different number of observations" << endl << endl << errorExit;
	else
		err << "file dimensions are compatible\n";
// are observation names the same?
	unsigned long int cmpfail = 0, i = 0;
	while (!cmpfail && i<indata1.data_type.nobservations) {
		cmpfail = strcmp(indata1.observation_names[i].name,indata2.observation_names[i].name);
		i++;
	}
	if (cmpfail){
		msg << "observation names are not the same in files '" << ifname1;
		msg << "' and '" << ifname2 << "', observation # " << i <<", names are '";
		msg << indata1.observation_names[i-1].name;
		msg << "' and '" << indata2.observation_names[i-1].name;
		msg << "' (only first shown); name will be taken from the first file\n";
	} else {
		msg << "observation names are the same in files '" << ifname1 << "' and '"<< ifname2 << "'\n\n";
	}
	unsigned long int out_nvars = indata1.data_type.nvariables + indata2.data_type.nvariables;
	unsigned long int out_nobs = indata1.data_type.nobservations;
	dbg << "initalizing FVF-file '" << ofname << "'...\n";
	initialize_empty_file(ofname, out_nvars, out_nobs, FLOAT, true);
	dbg << "writing out the data ... \n";

	filevector outdata(ofname, cachesize);

// copy observation names from the first object
	for (i=0;i<indata1.data_type.nobservations;i++)
	    outdata.observation_names[i] = indata1.observation_names[i];

// copy var names and data
	float * tmpdat = new (nothrow) float [outdata.data_type.nobservations];
	if (!tmpdat) err << "can not allocate memory for tmpdat" << endl << endl << errorExit;
	for (unsigned long int i=0;i<indata1.data_type.nvariables;i++)
	{
		outdata.variable_names[i] = indata1.variable_names[i];
		indata1.readVariable(i,tmpdat);
		outdata.writeVariable(i,tmpdat);
	}

	for (i=indata1.data_type.nvariables;i<(indata1.data_type.nvariables+indata2.data_type.nvariables);i++)
	{
		outdata.variable_names[i] = indata2.variable_names[i-indata1.data_type.nvariables];
		indata2.readVariable(i-indata1.data_type.nvariables,tmpdat);
		outdata.writeVariable(i,tmpdat);
	} 

	return(1);
}
