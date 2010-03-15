#include <sys/stat.h>
#include <string.h>
#include <iostream>

using namespace std;

#include "filevector.h"
#include "frutil.h"

void filevector::saveIndexFile() {
  indexFile.seekp(0, ios::beg);
  indexFile.write((char*)&data_type, sizeof(data_type));
  indexFile.seekp(sizeof(data_type), ios::beg);
  if (sizeof(fixedchar)*data_type.nobservations > INT_MAX) errorLog << "sizeof(fixedchar)*data_type.nobservations > INT_MAX" << endl << endl << errorExit;
  indexFile.write((char*)observation_names, sizeof(fixedchar)*data_type.nobservations);
  indexFile.seekp(sizeof(data_type)+sizeof(fixedchar)*data_type.nobservations, ios::beg);
  if (sizeof(fixedchar)*data_type.nvariables > INT_MAX) errorLog << "sizeof(fixedchar)*data_type.nvariables > INT_MAX" << endl << endl << errorExit;
  indexFile.write((char*)variable_names,sizeof(fixedchar)*data_type.nvariables);
}

void filevector::freeResources()
{
  dbg << "Closing filevector" << endl;
  saveIndexFile();
  delete [] char_buffer;
  delete [] observation_names;
  delete [] variable_names;
  indexFile.close();
  dataFile.close();
}

filevector::~filevector()
{
    freeResources();
}

void filevector::initialize(string filename_toload, unsigned long int cachesizeMb)
{
    dbg << "Opening filevector '" << filename_toload.c_str() <<"'."<< endl;

    if (sizeof(unsigned long int) != 8) {
        errorLog << "You appear to work on 32-bit system. Large files are not supported.\n";
    }

    index_filename = extract_base_file_name(filename_toload) + FILEVECTOR_INDEX_FILE_SUFFIX;
    data_filename = extract_base_file_name(filename_toload) + FILEVECTOR_DATA_FILE_SUFFIX;

    if(!file_exists(index_filename)) {
        errorLog <<"Index file not exists: " <<  index_filename << endl << errorExit;
    }

    data_filename = extract_base_file_name(filename_toload) + FILEVECTOR_DATA_FILE_SUFFIX;
    if(!file_exists(data_filename))
        errorLog <<"Data file not exists: " <<  data_filename.c_str() << endl <<errorExit;

    struct stat data_filestatus;
    stat(data_filename.c_str(), &data_filestatus);

    struct stat index_filestatus;
    stat(index_filename.c_str(), &index_filestatus);

    indexFile.open(index_filename.c_str(), ios::out | ios::in | ios::binary);
    if (!indexFile) {
        errorLog << "Opening file "<< index_filename <<" for write & read failed\n" << errorExit;
    }

    dataFile.open(data_filename.c_str(), ios::out | ios::in | ios::binary);
    if (!dataFile) {
        errorLog << "Opening file "<< data_filename << " for write & read failed\n" << errorExit;
    }

	indexFile.read((char*)&data_type, sizeof(data_type));
	if (!indexFile){
        errorLog << "Failed to read datainfo from file:" << index_filename << endl;
    }

    // some integrity checks
	if (getElementSize() != data_type.bytes_per_record) {
		errorLog << "System data type size ("<<getElementSize();
		errorLog <<") and file data type size ("<<data_type.bytes_per_record<<") do not match.\n";
	}

	//!!! nelements should actually be long to ensure !!!
	if (data_type.nelements != (data_type.nobservations*data_type.nvariables)) {
		errorLog << "Number of variables ("<<data_type.nvariables;
		errorLog << ") and observations ("<<data_type.nobservations<<") do not multiply to nelements";
		errorLog << "("<< data_type.nelements<<") (file integrity issue?)\n";
		errorLog << errorExit;
	}

	if ((data_type.bytes_per_record != (data_type.bits_per_record/8)) ||
	     ((data_type.bits_per_record % 8) != 0) || (data_type.bits_per_record < 8)) {
		errorLog << "Size in bytes/bits do not match or bit-size of char !=8 or ";
		errorLog << "non-byte recods (file integrity issue?)" << errorExit;
	}

  	header_size = sizeof(data_type) + sizeof(fixedchar)*(data_type.nvariables+data_type.nobservations);
    if(header_size != index_filestatus.st_size) {
        errorLog << "Index file size(" << index_filestatus.st_size << ") differs from the expected(";
        errorLog << header_size <<")"<< endl << errorExit;
    }

	// temp fix because nelements is not yet long ... !!!
    //	unsigned long int estimated_size = data_type.bytes_per_record*data_type.nelements + header_size;
	unsigned long int estimated_size =
			(unsigned long int) data_type.bytes_per_record *
			(unsigned long int) data_type.nvariables *
			(unsigned long int) data_type.nobservations;

	if (estimated_size != data_filestatus.st_size) {
        errorLog << "Data file size (" << data_filestatus.st_size;
        errorLog << ") differ from the expected ("<<estimated_size<<") [";
        errorLog << data_type.nvariables<<","<<data_type.nobservations<<"]";
        errorLog << errorExit;
    }

    // read in variable and observation names
	variable_names = new (nothrow) fixedchar [data_type.nvariables];
	if (!variable_names) errorLog << "can not get RAM for variable names" << errorExit;
	observation_names = new (nothrow) fixedchar [data_type.nobservations];
	if (!observation_names) errorLog << "can not get RAM for observation names" << errorExit;
	indexFile.seekg(sizeof(data_type), ios::beg);
	for (unsigned long int i=0;i<data_type.nobservations;i++)
		indexFile.read((char*)(observation_names+i),sizeof(fixedchar));
	for (unsigned long int i=0;i<data_type.nvariables;i++)
		indexFile.read((char*)(variable_names+i),sizeof(fixedchar));

    setCacheSizeInMb(cachesizeMb);
    update_cache(0);
    dbg << "Filevector " << filename_toload << " opened." << endl;
}

unsigned long int filevector::getCacheSizeInMb() {
	return cache_size_Mb;
}

void filevector::setCacheSizeInMb( unsigned long int cachesizeMb ) {
// figure out cache size
	cache_size_Mb = cachesizeMb;
	cache_size_nvars = (unsigned long int) 1024*1024*cache_size_Mb/(data_type.nobservations*data_type.bytes_per_record);
	if (cache_size_nvars<1) {
		cache_size_Mb = (long unsigned int) ceil(
				(float) data_type.nobservations*data_type.bytes_per_record/(1024.*1024.)
				);
		cache_size_nvars = 1;
	} else if (cache_size_nvars>data_type.nvariables) {
		cache_size_Mb = (long unsigned int) ceil(
				(float) data_type.nvariables*data_type.nobservations*data_type.bytes_per_record/(1024.*1024.)
				);
		cache_size_nvars = data_type.nvariables;
	}
	cache_size_bytes = cache_size_nvars*data_type.bytes_per_record*data_type.nobservations*sizeof(char);

    //free previously allocated memory
    if(char_buffer !=0 )
        delete char_buffer;
    // get memory for the cache
	char_buffer = new (nothrow) char [cache_size_bytes];
	if (!char_buffer)
		errorLog << "failed to get memory for cache" << endl << errorExit;
	max_buffer_size_bytes = INT_MAX;

	//don't read cache after resizing,
	//it will be updated on next read operation from desired position
	in_cache_from = 0;
	in_cache_to = 0;
}

void filevector::update_cache(unsigned long int from_var)
{
	unsigned long int current_cache_size_bytes = cache_size_bytes;
	in_cache_from = from_var;
	in_cache_to = from_var + cache_size_nvars - 1;
	if (in_cache_to >= data_type.nvariables) {
		in_cache_to = data_type.nvariables-1;
		current_cache_size_bytes = (in_cache_to-in_cache_from+1)*
					   data_type.bytes_per_record*data_type.nobservations*sizeof(char);
	}
	deepDbg << "Updating cache: " << in_cache_from << " - " << in_cache_to << "\n";
	unsigned long int internal_from = in_cache_from*data_type.nobservations*data_type.bytes_per_record*sizeof(char);
	dataFile.seekg(internal_from, ios::beg);
	if (current_cache_size_bytes <= max_buffer_size_bytes) {
		dataFile.read((char*)char_buffer,current_cache_size_bytes);
		if (!dataFile) {
		    errorLog << "Failed to read cache from file '"<< data_filename <<"'\n" << errorExit;
		}
	} else {
// cache size is bigger than what we can read in one go ... read in blocks
		unsigned long nbytes_togo = current_cache_size_bytes;
		unsigned long nbytes_finished = 0;
		while (nbytes_togo>0)
		if (nbytes_togo > max_buffer_size_bytes) {
			dataFile.read((char*)(char_buffer+nbytes_finished),max_buffer_size_bytes);
			if (!dataFile) {
			    errorLog << "Failed to read cache from file '"<<data_filename<<"'\n"<<errorExit;
			}
			nbytes_finished += max_buffer_size_bytes;
			nbytes_togo -= max_buffer_size_bytes;
		} else {
			dataFile.read((char*)(char_buffer+nbytes_finished),nbytes_togo);
			if (!dataFile) {
			    errorLog << "Failed to read cache from file '"<<data_filename<<"'\n"<<errorExit;
			}
			nbytes_finished += nbytes_togo;
			nbytes_togo -= nbytes_togo;
		}
	}
	cached_data = char_buffer;
}

void filevector::setUpdateNamesOnWrite(bool bUpdate) {
    updateNamesOnWrite = bUpdate;
}

void filevector::writeVariableName(unsigned long int nvar, fixedchar name)
{
	if (nvar>=data_type.nvariables) {
	    errorLog << "Trying to set name of obs out of range (" << nvar << ")\n\n" << endl << errorExit;
	}
	if (updateNamesOnWrite){
	    indexFile.seekp(sizeof(data_type) + sizeof(fixedchar)*(nvar + data_type.nobservations), ios::beg);
	    indexFile.write((char*)&name, sizeof(data_type));
	}
	variable_names[nvar] = name;
}

void filevector::writeObservationName(unsigned long int nobs, fixedchar name)
{
	if (nobs>=data_type.nobservations) {
	    errorLog << "Trying to set name of vars out of range (" << nobs << ")\n\n" << endl << errorExit;
	}
	if (updateNamesOnWrite){
	    indexFile.seekp(sizeof(data_type) + sizeof(fixedchar)*(nobs), ios::beg);
	    indexFile.write((char*)&name, sizeof(data_type));
    }
    observation_names[nobs] = name;
}

fixedchar filevector::readVariableName(unsigned long int nvar)
{
	if (nvar>=data_type.nvariables) {
	    errorLog << "trying to get name of var out of range" << errorExit;
	}
	return(variable_names[nvar]);
}

fixedchar filevector::readObservationName(unsigned long int nobs)
{
	if (nobs>=data_type.nobservations) {
	    errorLog << "trying to get name of obs out of range" << errorExit;
	}
	return(observation_names[nobs]);
}

// can read single variable
void filevector::readVariable(unsigned long int nvar, void * outvec)
{
	if (nvar>=data_type.nvariables) {
	    errorLog << "Variable number out of range (" << nvar << " >= " << data_type.nvariables <<")"<<endl << errorExit;
	}
	if (in_cache_to > 0 && nvar >= in_cache_from && nvar <= in_cache_to) {
		unsigned long int offset = (nvar - in_cache_from)*data_type.nobservations*getElementSize();
		memcpy(outvec,cached_data+offset,getElementSize()*(data_type.nobservations));
    } else {
		update_cache(nvar);
        memcpy(outvec,cached_data,getElementSize()*data_type.nobservations);
	}
}

void filevector::readObservation(unsigned long int nobs, void *outvec)
{
	char * tmpdata = new (nothrow) char [getNumObservations()*getElementSize()];
	if(!tmpdata)
		errorLog << "readObservation: cannot allocate tmpdata" << errorExit;

	for( int i = 0; i< getNumVariables(); i++)
	{
		readVariable(i, tmpdata);
		memcpy((char*)outvec+i*getElementSize(),tmpdata+getElementSize()*nobs,getElementSize());
	}
	delete[] tmpdata;
}

void filevector::writeObservation(unsigned long int nobs, void * invec)
{
    if (readOnly) {
        errorLog << "Trying to write to the readonly file." << errorExit;
    }
	for( int i = 0; i< getNumVariables(); i++)
	{
		writeElement( i, nobs, (char*)invec+ i*getElementSize() );
	}
}

// can write single variable
void filevector::writeVariable(unsigned long int nvar, void * datavec)
{
    if (readOnly) {
        errorLog << "Trying to write to the readonly file." << errorExit;
    }
	unsigned long int pos = nrnc_to_nelem(nvar, 0);
	dataFile.seekp(pos*getElementSize(), ios::beg);
	dataFile.write((char*)datavec,getElementSize()*data_type.nobservations);
	if (!dataFile) {
	    errorLog <<"failed to write to data file\n"<<errorExit;
	}

	//update data in cache
 	deepDbg << "var:"<< nvar << ",cache from :"<< in_cache_from << ", to: "<< in_cache_to  << endl;

	if (nvar >= in_cache_from && nvar <= in_cache_to)
	{
	    unsigned long int offset = (nvar - in_cache_from)*data_type.nobservations*getElementSize();
 	    memcpy(cached_data + offset,datavec,getElementSize()*data_type.nobservations);
	}
}

unsigned long int filevector::nrnc_to_nelem(unsigned long int nvar, unsigned long int nobs)
{
    if (nvar >= data_type.nvariables) {
        errorLog << "Variable number out of bounds (" << nvar << " >= " <<  data_type.nvariables << ")" << endl << errorExit;
    }
    if (nobs >= data_type.nobservations) {
        errorLog << "Observation number out of bounds (" << nobs << " >= " <<  data_type.nvariables << ")" << endl << errorExit;
    }
    return( nvar * data_type.nobservations + nobs );
}

// should only be used for reading single random elements!

void filevector::readElement(unsigned long int nvar, unsigned long int nobs, void * out)
{
    deepDbg << "filevector.readElement(" << nvar << "," << nobs << ");" << endl;
    unsigned long int pos = nrnc_to_nelem(nvar, nobs);
    dataFile.seekg(pos*getElementSize(), ios::beg);
    dataFile.read((char*)out,getElementSize());
}

void filevector::writeElement(unsigned long int nvar, unsigned long int nobs, void* data)
{
    if (readOnly) {
        errorLog << "Trying to write to the readonly file." << errorExit;
    }
    deepDbg << "filevector.writeElement(" << nvar << "," << nobs << ");" << endl;
    unsigned long int pos = nrnc_to_nelem(nvar, nobs);
    dataFile.seekp(pos*getElementSize(), ios::beg);
    dataFile.write((char*)data,getElementSize());
    dataFile.flush();

    if (nvar >= in_cache_from && nvar <= in_cache_to)
    {
	unsigned long int offset = (nvar - in_cache_from)*data_type.nobservations*getElementSize() + nobs *getElementSize();
	memcpy(cached_data+offset,data,getElementSize() );
    }
}

unsigned int filevector::getNumVariables()
{
   return data_type.nvariables;
}

unsigned int filevector::getNumObservations()
{
   return data_type.nobservations;
}

void filevector::saveAs( string newFilename )
{
    initialize_empty_file( (char *)newFilename.c_str(), getNumVariables(), getNumObservations(), data_type.type, true);
    filevector *outdata = new filevector( newFilename, 64 );//todo which size for cache to use?

    // copy observation names from the first object
  	for (unsigned long int i=0;i<getNumObservations();i++)
  	    outdata->writeObservationName( i, readObservationName( i ) );

    char * tmpvariable = new (nothrow) char[getNumObservations()*getElementSize()];
    if (!tmpvariable) errorLog << "can not allocate memory for tmpvariable" << endl << endl << errorExit;

    for (unsigned long int i=0 ; i<getNumVariables();i++)
    {
        //write var names
        outdata->writeVariableName( i, readVariableName(i));
        //write variables
        readVariable(i,tmpvariable);
        outdata->writeVariable(i,tmpvariable);
    }
    delete outdata;
    delete [] tmpvariable;
}

void filevector::saveVariablesAs( string newFilename, unsigned long int nvars, unsigned long int *varindexes) {
    initialize_empty_file( (char *)newFilename.c_str(), nvars, getNumObservations(), data_type.type, true);
    filevector outdata( newFilename, 64 );//todo which size for cache to use?

    // copy observation names from the first object
  	for (unsigned long int i=0;i<getNumObservations();i++)
  	    outdata.writeObservationName( i, readObservationName( i ) );

    char * tmpvariable = new (nothrow) char[getNumObservations()*getElementSize()];
    if (!tmpvariable) errorLog << "can not allocate memory for tmpvariable" << endl << endl << errorExit;

    for ( unsigned long int i=0 ; i<nvars ; i++ )
    {
        unsigned long int selected_index =  varindexes[i];
        //write var names
        outdata.writeVariableName( i, readVariableName(selected_index ));
        //write variables
        readVariable(selected_index,tmpvariable);
        outdata.writeVariable(i,tmpvariable);
    }

    delete [] tmpvariable;
}

void filevector::saveObservationsAs( string newFilename, unsigned long int nobss, unsigned long int * obsindexes)
{
    if (headerOrDataExists(newFilename))
    {
        errorLog << "File " << newFilename <<" already exists" << endl << errorExit;
    }
    initialize_empty_file( (char *)newFilename.c_str(), getNumVariables(), nobss, data_type.type,true);
    filevector outdata( newFilename, 64 );//todo which size for cache to use?

    // copy observation names from the first object
  	for( unsigned long int i=0 ; i < nobss ; i++ )
  	    outdata.writeObservationName( i, readObservationName( obsindexes[i] ) );

    char * in_variable = new (nothrow) char[getNumObservations()*getElementSize()];
    if (!in_variable) errorLog << "can not allocate memory for tmpvariable" << endl << endl << errorExit;

    char * out_variable = new (nothrow) char[nobss*getElementSize()];
    if (!out_variable) errorLog << "can not allocate memory for tmpvariable" << endl << endl << errorExit;
    for ( unsigned long int i=0 ; i<getNumVariables() ; i++ )
    {
        //write var names
        outdata.writeVariableName( i, readVariableName(i));
        //write variables
        readVariable(i, in_variable);
        copyVariable(out_variable, in_variable, nobss, obsindexes);
        outdata.writeVariable(i,out_variable);
    }

    delete [] in_variable;
    delete [] out_variable;
}

/*
* copy elements from "from" array to "to" array, according to "n" and "indexes" parameters
*/
void filevector::copyVariable(char* to, char* from, int n, unsigned long int * indexes ) {
	for ( int j=0 ; j<n ; j++ )	{
		//copy only selected observations to out_variable  from in_variable
		int read_offset = indexes[j]*getElementSize();
		if(read_offset + getElementSize() > getNumObservations() * getElementSize()) {
		    errorLog << "When saving selected observations: index in obsindexes(" <<indexes[j];
		    errorLog << ") is out of range, source nobs is " << getNumObservations()<< endl;
		    errorLog << errorExit;
		}
		memcpy(to + j*getElementSize(),from + read_offset,getElementSize());
	}
}

void filevector::saveAs(string newFilename, unsigned long int nvars, unsigned long int nobss,
    unsigned long int * varindexes, unsigned long int * obsindexes) {
    if (headerOrDataExists(newFilename)) {
        errorLog << "File "<< newFilename <<" already exists." << endl;
    }
    initialize_empty_file( (char *)newFilename.c_str(), nvars, nobss, data_type.type, true);
    filevector outdata( newFilename, 64 );//todo which size for cache to use?

    // copy observation names from the first object
	for( unsigned long int i=0 ; i < nobss ; i++ )
		outdata.writeObservationName( i, readObservationName( obsindexes[i] ) );

    char * out_variable = new (nothrow) char[nobss*getElementSize()];
    if (!out_variable) errorLog << "can not allocate memory for out_variable" << errorExit;

	char * in_variable = new (nothrow) char[getNumObservations()*getElementSize()];
	if (!in_variable) errorLog << "can not allocate memory for in_variable" << endl << endl << errorExit;


	for( unsigned long int i=0 ; i<nvars ; i++ )
	{
		unsigned long int selected_index =  varindexes[i];
		//write var names
		outdata.writeVariableName( i, readVariableName(selected_index ));
		//write variables
		readVariable(selected_index,in_variable);
		copyVariable(out_variable, in_variable, nobss, obsindexes);
		outdata.writeVariable(i,out_variable );
	}

	delete[] in_variable;
	delete[] out_variable;
}

void filevector::saveAsText(string newFilename, unsigned long int nvars, unsigned long int nobss,
    unsigned long int * varindexes, unsigned long int * obsindexes) {

    dbg << "nvars = " << nvars << endl;
    dbg << "nobs = " << nobss << endl;

    ofstream textfile(newFilename.c_str(), ios::out);

    // copy observation names from the first object
	for( unsigned long int i=0 ; i < nobss ; i++ ){
	    fixedchar fc = readObservationName( obsindexes[i] ) ;
        textfile << fc.name << " ";
    }

    textfile << endl;

    char * out_variable = new (nothrow) char[nobss*getElementSize()];
    if (!out_variable)
            errorLog << "can not allocate memory for out_variable" << errorExit;

	char * in_variable = new (nothrow) char[getNumObservations()*getElementSize()];
		if (!in_variable)
		    errorLog << "can not allocate memory for in_variable" << endl << endl << errorExit;

	for( unsigned long int i=0 ; i<nvars ; i++ )
    {
        dbg << "Writing var " << i << " of " << nvars << endl;
		unsigned long int selected_index = varindexes[i];
    	//write var names
    	fixedchar fc = readVariableName(selected_index);
		textfile << fc.name << " ";
		//write variables
		readVariable(selected_index, in_variable);
		copyVariable(out_variable, in_variable, nobss, obsindexes);

	    for( unsigned long int j=0 ; j<nobss ; j++ )
	    {
	        double x;
	        performCast(x, &out_variable[j*getElementSize()],getElementType());
		    textfile << x << " ";
		}
		textfile << endl;
	}

	delete[] in_variable;
	delete[] out_variable;
}

short unsigned filevector::getElementSize(){
    return calcDataSize(data_type.type);
}

short unsigned filevector::getElementType(){
    return data_type.type;
}

void filevector::addVariable(void * invec, string varname)
{
    if (readOnly) {
        errorLog << "Trying to write to the readonly file." << errorExit;
    }
      data_type.nvariables++;
      data_type.nelements = data_type.nvariables*data_type.nobservations;//recalculate

      fixedchar * new_variable_names = new (nothrow)fixedchar[data_type.nvariables];
      if (!new_variable_names)
          errorLog << "can not allocate memory in addVariable()" << errorExit;

      //reallocate greater array for var names
      memcpy(new_variable_names,variable_names,sizeof(fixedchar)*(data_type.nvariables-1));
      fixedchar _fc_varname(varname);
      new_variable_names[data_type.nvariables - 1] = _fc_varname;
      fixedchar * oldvar_names = variable_names;
      variable_names = new_variable_names;
      delete[] oldvar_names;

      writeVariable(data_type.nvariables - 1,invec);
}



