#include <sys/stat.h>
#include <string.h>
#include <iostream>

using namespace std;

#include "filevector.h"
#include "frutil.h"

void filevector::saveIndexFile() {
  index_file.seekp(0, ios::beg);
  index_file.write((char*)&data_type, sizeof(data_type));
  index_file.seekp(sizeof(data_type), ios::beg);
  if (sizeof(fixedchar)*data_type.nobservations > INT_MAX) error("sizeof(fixedchar)*data_type.nobservations > INT_MAX\n\n");
  index_file.write((char*)observation_names, sizeof(fixedchar)*data_type.nobservations);
  index_file.seekp(sizeof(data_type)+sizeof(fixedchar)*data_type.nobservations, ios::beg);
  if (sizeof(fixedchar)*data_type.nvariables > INT_MAX) error("sizeof(fixedchar)*data_type.nvariables > INT_MAX\n\n");
  index_file.write((char*)variable_names,sizeof(fixedchar)*data_type.nvariables);
}

void filevector::freeResources()
{
  dbg << "Closing filevector" << nl;
  saveIndexFile();
  delete [] char_buffer;
  delete [] observation_names;
  delete [] variable_names;
  index_file.close();
  data_file.close();
}

filevector::~filevector()
{
    freeResources();
}

void filevector::initialize(string filename_toload, unsigned long int cachesizeMb)
{
    dbg << "Opening filevector '" << filename_toload.c_str() <<"'."<< nl;

    if (sizeof(unsigned long int) != 8) warning("you appear to work on 32-bit system... large files not supported\n");

    index_filename = extract_base_file_name(filename_toload) + FILEVECTOR_INDEX_FILE_SUFFIX;
    data_filename = extract_base_file_name(filename_toload) + FILEVECTOR_DATA_FILE_SUFFIX;

    if(!file_exists(index_filename))
        error("index file not exists: %s", index_filename.c_str());

    data_filename = extract_base_file_name(filename_toload) + FILEVECTOR_DATA_FILE_SUFFIX;
    if(!file_exists(data_filename))
        error("data file not exists: %s", data_filename.c_str());

    struct stat data_filestatus;
    stat(data_filename.c_str(), &data_filestatus);

    struct stat index_filestatus;
    stat(index_filename.c_str(), &index_filestatus);

    index_file.open(index_filename.c_str(), ios::out | ios::in | ios::binary);
    if (!index_file)
      error("opening file %s for write & read failed\n",index_filename.c_str());

    data_file.open(data_filename.c_str(), ios::out | ios::in | ios::binary);
    if (!data_file)
      error("opening file %s for write & read failed\n",data_filename.c_str());

	index_file.read((char*)&data_type, sizeof(data_type));
	if (!index_file)
        error("failed to read datainfo from file '%s'\n",index_filename.c_str());

    // some integrity checks
	if (getElementSize() != data_type.bytes_per_record)
		error("system data type size (%d) and file data type size (%d) do not match\n",
		    getElementSize(),data_type.bytes_per_record);

	//!!! nelements should actually be long to ensure !!!
	if (data_type.nelements != (data_type.nobservations*data_type.nvariables))
		error("number of variables (%lu) and observations (%lu) do not multiply to nelements (%lu) (file integrity issue?)\n",
			data_type.nvariables, data_type.nobservations, data_type.nelements);

	if ((data_type.bytes_per_record != (data_type.bits_per_record/8)) ||
	     ((data_type.bits_per_record % 8) != 0) || (data_type.bits_per_record < 8))
		perror("size in bytes/bits do not match or bit-size of char !=8 or non-byte recods (file integrity issue?)");

  	header_size = sizeof(data_type) + sizeof(fixedchar)*(data_type.nvariables+data_type.nobservations);
    if(header_size != index_filestatus.st_size)
        error("index file size(%lu) differs from the expected(%lu)", index_filestatus.st_size, header_size );

	// temp fix because nelements is not yet long ... !!!
    //	unsigned long int estimated_size = data_type.bytes_per_record*data_type.nelements + header_size;
	unsigned long int estimated_size =
			(unsigned long int) data_type.bytes_per_record *
			(unsigned long int) data_type.nvariables *
			(unsigned long int) data_type.nobservations;
	if (estimated_size != data_filestatus.st_size)
        error("data file size (%lu) differ from the expected (%lu) [%lu,%lu]",
		    data_filestatus.st_size,estimated_size,data_type.nvariables,data_type.nobservations);

    // read in variable and observation names
	variable_names = new (nothrow) fixedchar [data_type.nvariables];
	if (!variable_names) error("can not get RAM for variable names");
	observation_names = new (nothrow) fixedchar [data_type.nobservations];
	if (!observation_names) error("can not get RAM for observation names");
	index_file.seekg(sizeof(data_type), ios::beg);
	for (unsigned long int i=0;i<data_type.nobservations;i++)
		index_file.read((char*)(observation_names+i),sizeof(fixedchar));
	for (unsigned long int i=0;i<data_type.nvariables;i++)
		index_file.read((char*)(variable_names+i),sizeof(fixedchar));

    setCacheSizeInMb(cachesizeMb);
    update_cache(0);
    dbg << "Filevector " << filename_toload << " opened." << nl;
}

unsigned long int filevector::getCacheSizeInMb() {
	return cache_size_Mb;
}

void filevector::setCacheSizeInMb( unsigned long int cachesizeMb ) {
// figure out cache size
	cache_size_Mb = cachesizeMb;
	cache_size_nvars = (unsigned long int) 1024*1024*cache_size_Mb/(data_type.nobservations*data_type.bytes_per_record);
	if (cache_size_nvars<1) {
//		message("attempting to set cache size to 1 var (%f Mb)\n", (float) data_type.nobservations*data_type.bytes_per_record/(1024.*1024.));
		cache_size_Mb = (long unsigned int) ceil(
				(float) data_type.nobservations*data_type.bytes_per_record/(1024.*1024.)
				);
		cache_size_nvars = 1;
	} else if (cache_size_nvars>data_type.nvariables) {
//		message("attempting to cache all the data (%u variables, %f Mb)\n", data_type.nvariables, (float) data_type.nvariables*data_type.nobservations*data_type.bytes_per_record/(1024.*1024.));
		cache_size_Mb = (long unsigned int) ceil(
				(float) data_type.nvariables*data_type.nobservations*data_type.bytes_per_record/(1024.*1024.)
				);
		cache_size_nvars = data_type.nvariables;
	} else {
//		message("attempting to cache specified amount of data (%u variables, %f Mb)\n", cache_size_nvars, (float) cache_size_nvars*data_type.nobservations*data_type.bytes_per_record/(1024.*1024.));
	}
	cache_size_bytes = cache_size_nvars*data_type.bytes_per_record*data_type.nobservations*sizeof(char);

    //free previously allocated memory
    if(char_buffer !=0 )
        delete char_buffer;
    // get memory for the cache
	char_buffer = new (nothrow) char [cache_size_bytes];
	if (!char_buffer)
		error("failed to get memory for cache\n");
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
//	dbg << "updating cache: " << in_cache_from << " - " << in_cache_to << "\n";
	unsigned long int internal_from = in_cache_from*data_type.nobservations*data_type.bytes_per_record*sizeof(char);
//	dbg << "position = " << internal_from << "\n";
	data_file.seekg(internal_from, ios::beg);
	if (current_cache_size_bytes <= max_buffer_size_bytes) {
		data_file.read((char*)char_buffer,current_cache_size_bytes);
		if (!data_file) error("failed to read cache from file '%s'\n",data_filename.c_str());
	} else {
// cache size is bigger than what we can read in one go ... read in blocks
		unsigned long int nbytes_togo = current_cache_size_bytes;
		unsigned long int nbytes_finished = 0;
		while (nbytes_togo>0)
		if (nbytes_togo > max_buffer_size_bytes) {
			data_file.read((char*)(char_buffer+nbytes_finished),max_buffer_size_bytes);
			if (!data_file) error("failed to read cache from file '%s'\n",data_filename.c_str());
			nbytes_finished += max_buffer_size_bytes;
			nbytes_togo -= max_buffer_size_bytes;
		} else {
			data_file.read((char*)(char_buffer+nbytes_finished),nbytes_togo);
			if (!data_file) error("failed to read cache from file '%s'\n",data_filename.c_str());
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
	if (nvar>=data_type.nvariables) error("trying to set name of obs out of range (%lu)\n\n",nvar);
	if (updateNamesOnWrite){
	    index_file.seekp(sizeof(data_type) + sizeof(fixedchar)*(nvar + data_type.nobservations), ios::beg);
	    index_file.write((char*)&name, sizeof(data_type));
	}
	variable_names[nvar] = name;
}

void filevector::writeObservationName(unsigned long int nobs, fixedchar name)
{
	if (nobs>=data_type.nobservations) error("trying to set name of vars out of range (%lu)\n\n",nobs);
	if (updateNamesOnWrite){
	    index_file.seekp(sizeof(data_type) + sizeof(fixedchar)*(nobs), ios::beg);
	    index_file.write((char*)&name, sizeof(data_type));
    }
    observation_names[nobs] = name;
}

fixedchar filevector::readVariableName(unsigned long int nvar)
{
	if (nvar>=data_type.nvariables) error("trying to get name of var out of range");
	return(variable_names[nvar]);
}

fixedchar filevector::readObservationName(unsigned long int nobs)
{
	if (nobs>=data_type.nobservations) error("trying to get name of obs out of range");
	return(observation_names[nobs]);
}

// can read single variable
void filevector::readVariable(unsigned long int nvar, void * outvec)
{
	if (nvar>=data_type.nvariables) error("nvar out of range (%lu >= %lu)",nvar,data_type.nvariables);
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
		error("readObservation: cannot allocate tmpdata");

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
        error("Trying to write to the readonly file.");
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
        error("Trying to write to the readonly file.");
    }
	unsigned long int pos = nrnc_to_nelem(nvar, 0);
	data_file.seekp(pos*getElementSize(), ios::beg);
	data_file.write((char*)datavec,getElementSize()*data_type.nobservations);
	if (!data_file) error ("failed to write to data file\n");

	//update data in cache
 	dbg << "var:"<< nvar << ",cache from :"<< in_cache_from << ", to: "<< in_cache_to  << nl;

	if (nvar >= in_cache_from && nvar <= in_cache_to)
	{
	    unsigned long int offset = (nvar - in_cache_from)*data_type.nobservations*getElementSize();
 	    memcpy(cached_data + offset,datavec,getElementSize()*data_type.nobservations);
	}
}

unsigned long int filevector::nrnc_to_nelem(unsigned long int nvar, unsigned long int nobs)
{
    if (nvar >= data_type.nvariables || nobs >= data_type.nobservations)
        error("nvar >= real or nobs >= real (%u >= %u || %u >= %u)\n",nvar, data_type.nvariables, nobs, data_type.nobservations);
    return( nvar * data_type.nobservations + nobs );
}

// should only be used for reading single random elements!

void filevector::readElement(unsigned long int nvar, unsigned long int nobs, void * out)
{
    //todo use cache
    unsigned long int pos = nrnc_to_nelem(nvar, nobs);
    data_file.seekg(pos*getElementSize(), ios::beg);
    data_file.read((char*)out,getElementSize());
}

void filevector::writeElement(unsigned long int nvar, unsigned long int nobs, void* data)
{
    if (readOnly) {
        error("Trying to write to the readonly file.");
    }
    unsigned long int pos = nrnc_to_nelem(nvar, nobs);
    data_file.seekp(pos*getElementSize(), ios::beg);
    data_file.write((char*)data,getElementSize());
    data_file.flush();

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
    if (!tmpvariable) error("can not allocate memory for tmpvariable\n\n");

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
    if (!tmpvariable) error("can not allocate memory for tmpvariable\n\n");

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
        error("File %s already exists", newFilename.c_str());
    }
    initialize_empty_file( (char *)newFilename.c_str(), getNumVariables(), nobss, data_type.type,true);
    filevector outdata( newFilename, 64 );//todo which size for cache to use?

    // copy observation names from the first object
  	for( unsigned long int i=0 ; i < nobss ; i++ )
  	    outdata.writeObservationName( i, readObservationName( obsindexes[i] ) );

    char * in_variable = new (nothrow) char[getNumObservations()*getElementSize()];
    if (!in_variable) error("can not allocate memory for tmpvariable\n\n");

    char * out_variable = new (nothrow) char[nobss*getElementSize()];
    if (!out_variable) error("can not allocate memory for tmpvariable\n\n");
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
void filevector::copyVariable(char* to, char* from, int n, unsigned long int * indexes )
{
	for ( int j=0 ; j<n ; j++ )
	{
		//copy only selected observations to out_variable  from in_variable
		int read_offset = indexes[j]*getElementSize();
		if(read_offset + getElementSize() > getNumObservations() * getElementSize())
		  error( "when saving selected observations: index in obsindexes(%d) is out of range, source nobs is %d",indexes[j],getNumObservations());
		memcpy(to + j*getElementSize(),from + read_offset,getElementSize());
	}
}

void filevector::saveAs(string newFilename, unsigned long int nvars, unsigned long int nobss,
    unsigned long int * varindexes, unsigned long int * obsindexes) {
    if (headerOrDataExists(newFilename)) {
        error("File %s already exists.", newFilename.c_str());
    }
    initialize_empty_file( (char *)newFilename.c_str(), nvars, nobss, data_type.type, true);
    filevector outdata( newFilename, 64 );//todo which size for cache to use?

    // copy observation names from the first object
	for( unsigned long int i=0 ; i < nobss ; i++ )
		outdata.writeObservationName( i, readObservationName( obsindexes[i] ) );

    char * out_variable = new (nothrow) char[nobss*getElementSize()];
    if (!out_variable) error("can not allocate memory for out_variable");

	char * in_variable = new (nothrow) char[getNumObservations()*getElementSize()];
	if (!in_variable) error("can not allocate memory for in_variable\n\n");


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

    dbg << "nvars = " << nvars << nl;
    dbg << "nobs = " << nobss << nl;

    ofstream textfile(newFilename.c_str(), ios::out);

    // copy observation names from the first object
	for( unsigned long int i=0 ; i < nobss ; i++ ){
	    fixedchar fc = readObservationName( obsindexes[i] ) ;
        textfile << fc.name << " ";
    }

    textfile << endl;

    char * out_variable = new (nothrow) char[nobss*getElementSize()];
    if (!out_variable)
            error("can not allocate memory for out_variable");

	char * in_variable = new (nothrow) char[getNumObservations()*getElementSize()];
		if (!in_variable)
		    error("can not allocate memory for in_variable\n\n");

	for( unsigned long int i=0 ; i<nvars ; i++ )
    {
        dbg << "Writing var " << i << " of " << nvars << nl;
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
        error("Trying to write to the readonly file.");
    }
      data_type.nvariables++;
      data_type.nelements = data_type.nvariables*data_type.nobservations;//recalculate

      fixedchar * new_variable_names = new (nothrow)fixedchar[data_type.nvariables];
      if (!new_variable_names)
          error("can not allocate memory in addVariable()");

      //reallocate greater array for var names
      memcpy(new_variable_names,variable_names,sizeof(fixedchar)*(data_type.nvariables-1));
      fixedchar _fc_varname(varname);
      new_variable_names[data_type.nvariables - 1] = _fc_varname;
      fixedchar * oldvar_names = variable_names;
      variable_names = new_variable_names;
      delete[] oldvar_names;

      writeVariable(data_type.nvariables - 1,invec);
}



