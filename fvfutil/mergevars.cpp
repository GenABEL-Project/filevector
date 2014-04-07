#include "mergevars.h"

int main(int argc, char * argv[])
{
    print_mergevars_welcome();

    if (argc < 4) print_mergevars_usage(argv[0]);

    char * ifname1 = argv[1], * ifname2 = argv[2], * ofname = argv[3];
    unsigned long cachesize = 64;
    if (argc > 4) cachesize = atoi(argv[4]);
    if (cachesize < 0)
    {
        errorLog << "cache size must be positive long integer"
                 << endl << endl << errorExit;
    }
    dbg << "Options in effect:\n";
    dbg << "\tINFILE1   = " << ifname1 << "\n";
    dbg << "\tINFILE2   = " << ifname2 << "\n";
    dbg << "\tOUTFILE   = " << ofname << "\n";
    dbg << "\tcachesize = " << cachesize << " Mb\n\n";

    FileVector indata1(ifname1, cachesize);
    FileVector indata2(ifname2, cachesize);

    // sanity checks: can we merge these files?
    // are dimensions compatible?
    if (indata1.data_type.nobservations != indata2.data_type.nobservations)
    {
        errorLog << "can not merge files with different number of observations"
                 << endl << endl << errorExit;
    }
    else
    {
        errorLog << "file dimensions are compatible\n";
    }
    // are observation names the same?
    unsigned long cmpfail = 0, i = 0;
    while (!cmpfail && i<indata1.data_type.nobservations) {
        cmpfail = strcmp(indata1.observationNames[i].name,
                         indata2.observationNames[i].name);
        i++;
    }
    if (cmpfail){
        msg << "observation names are not the same in files '" << ifname1;
        msg << "' and '" << ifname2 << "', observation # " << i
            << ", names are '";
        msg << indata1.observationNames[i-1].name;
        msg << "' and '" << indata2.observationNames[i-1].name;
        msg << "' (only first shown); name will be taken from the first file\n";
    } else {
        msg << "observation names are the same in files '" << ifname1
            << "' and '"<< ifname2 << "'\n\n";
    }
    unsigned long out_nvars = indata1.data_type.numVariables +
        indata2.data_type.numVariables;
    unsigned long out_nobs = indata1.data_type.nobservations;
    dbg << "initalizing FVF-file '" << ofname << "'...\n";
    initializeEmptyFile(ofname, out_nvars, out_nobs, FLOAT, true);
    dbg << "writing out the data ... \n";

    FileVector outdata(ofname, cachesize);

    // copy observation names from the first object
    for (i = 0; i < indata1.data_type.nobservations; i++)
    {
        outdata.observationNames[i] = indata1.observationNames[i];
    }

    // copy var names and data
    float * tmpdat = new (nothrow) float [outdata.data_type.nobservations];
    if (!tmpdat)
    {
        errorLog << "can not allocate memory for tmpdat"
                 << endl << endl << errorExit;
    }

    for (unsigned long i = 0; i < indata1.data_type.numVariables; i++)
    {
        outdata.variableNames[i] = indata1.variableNames[i];
        indata1.readVariable(i,tmpdat);
        outdata.writeVariable(i,tmpdat);
    }

    for (i = indata1.data_type.numVariables;
         i < (indata1.data_type.numVariables + indata2.data_type.numVariables);
         i++)
    {
        outdata.variableNames[i] =
            indata2.variableNames[i-indata1.data_type.numVariables];
        indata2.readVariable(i-indata1.data_type.numVariables, tmpdat);
        outdata.writeVariable(i, tmpdat);
    }

    return(1);
}
