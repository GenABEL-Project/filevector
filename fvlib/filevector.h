#ifndef __FRVECTOR__
#define __FRVECTOR__

#include <new>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>

#include "Logger.h"
#include "frutil.h"
#include "frerror.h"

#include "AbstractMatrix.h"

using namespace std;

#define DB_CREATE 1
#define DB_EXCL 2
#define DB_RDONLY 4

class filevector: public AbstractMatrix
{
public:
	string data_filename;
	string index_filename;
	fstream dataFile;
	fstream indexFile;
	fr_type data_type;
	// row and column names
	fixedchar * variable_names;
	fixedchar * observation_names;
	// size of header (descriptives + var/obs names)
	unsigned long int header_size;
	// cache size (Mb) requested by user
	unsigned long int cache_size_Mb;
	// cache size internal; these ones are exact and used internaly
	unsigned long int cache_size_nvars;
	unsigned long int cache_size_bytes;
	unsigned long int max_buffer_size_bytes;
	// which variables are now in cache
	unsigned long int in_cache_from;
	unsigned long int in_cache_to;
	char * cached_data;
	char * char_buffer;

	// prototypes
	filevector();
	~filevector();

	bool readOnly;
	bool updateNamesOnWrite;	

	filevector(string filename_toload, unsigned long int cachesizeMb)
	{
		readOnly = false;
		updateNamesOnWrite = false;
		char_buffer = 0;
		initialize(filename_toload, cachesizeMb);
	}

	filevector(string filename_toload, unsigned long int cachesizeMb, bool iReadOnly) : readOnly(iReadOnly)
    {
		updateNamesOnWrite = false;
		char_buffer = 0;
		initialize(filename_toload, cachesizeMb);
    }

	filevector(char *filename_toload, unsigned long int cachesizeMb)
	{
		updateNamesOnWrite = false;
		readOnly = false;
		string filename(filename_toload);
		char_buffer = 0;
		initialize(filename_toload, cachesizeMb);
	}

	filevector(char *filename_toload, unsigned long int cachesizeMb, bool iReadOnly) : readOnly(iReadOnly)
	{
		updateNamesOnWrite = false;
		string filename(filename_toload);
		char_buffer = 0;
		initialize(filename_toload, cachesizeMb);
	}

	// these ones are the actual used to initialize and free up
	void initialize(string filename_toload, unsigned long int cachesizeMb);
	// this one updates cache
	void update_cache(unsigned long int from_var);
	// gives element number from nvar & nobs
	unsigned long int nrnc_to_nelem(unsigned long int nvar, unsigned long int nobs);

	// getting and setting var/col names
	void writeVariableName(unsigned long int nvar, fixedchar name);
	void writeObservationName(unsigned long int nobs, fixedchar name);

	virtual unsigned int getNumVariables();
	virtual unsigned int getNumObservations();

	fixedchar readVariableName(unsigned long int nvar);
	fixedchar readObservationName(unsigned long int nobs);

	// USER FUNCTIONS
	// can read single variable
	void readVariable(unsigned long int nvar, void * outvec);

	void addVariable(void * invec, string varname);

	// should only be used for reading single random elements!
	void readElement(unsigned long int nvar, unsigned long int nobs, void * data);
	void writeVariable(unsigned long int nvar, void * datavec);
	void writeElement(unsigned long int nvar, unsigned long int nobs, void * data);

	void readObservation(unsigned long int nobs, void * outvec);
	void writeObservation(unsigned long int nobs, void * outvec);

	void saveAs( string newFilename );
	void saveVariablesAs( string newFilename, unsigned long int nvars, unsigned long int * varindexes);
	void saveObservationsAs( string newFilename, unsigned long int nobss, unsigned long int * obsindexes);
	void saveAs(string newFilename, unsigned long int nvars, unsigned long int nobss, unsigned long int * varindexes, unsigned long int * obsindexes);
	void saveAsText(string newFilename, unsigned long int nvars, unsigned long int nobss, unsigned long int * varindexes, unsigned long int * obsindexes);

	unsigned long int getCacheSizeInMb();
	void setCacheSizeInMb( unsigned long int cachesizeMb );

	virtual short unsigned getElementSize();
	virtual short unsigned getElementType();
	void saveIndexFile();

	virtual void setUpdateNamesOnWrite(bool bUpdate);

	// FOR FUTURE:
	// very slow one!
	//	DT * readObservation(unsigned long int nobs);
	// should only be used for reading single random elements!
	//	DT readElement(unsigned long int nelment);
private :
	void copyVariable(char * to, char * from, int n, unsigned long int * indexes );
	void freeResources();

};

//global variables
const string FILEVECTOR_DATA_FILE_SUFFIX=".fvd";
const string FILEVECTOR_INDEX_FILE_SUFFIX=".fvi";

#endif

