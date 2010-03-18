#include <sys/stat.h>
#include <string.h>
#include <iostream>

using namespace std;

#include "FileVector.h"
#include "frutil.h"

void FileVector::saveIndexFile() {
    indexFile.seekp(0, ios::beg);
    indexFile.write((char*)&fileHeader, sizeof(fileHeader));
    indexFile.seekp(sizeof(fileHeader), ios::beg);

    if (observationNames && variableNames) {
        indexFile.write((char*)observationNames, sizeof(fixedchar)*fileHeader.numObservations);
        indexFile.seekp(sizeof(fileHeader)+sizeof(fixedchar)*fileHeader.numObservations, ios::beg);
        indexFile.write((char*)variableNames,sizeof(fixedchar)*fileHeader.numVariables);
    }
}

FileVector::~FileVector() {
    dbg << "Closing FileVector" << endl;
    saveIndexFile();
    delete [] char_buffer;
    delete [] observationNames;
    delete [] variableNames;
    indexFile.close();
    dataFile.close();
}

void FileVector::initialize(string filename, unsigned long cachesizeMb) {
    dbg << "Opening FileVector '" << filename.c_str() <<"'."<< endl;

    if (sizeof(unsigned long) != 8) {
        errorLog << "You appear to work on 32-bit system. Large files are not supported.\n";
    }

    indexFilename = extract_base_file_name(filename) + FILEVECTOR_INDEX_FILE_SUFFIX;
    data_filename = extract_base_file_name(filename) + FILEVECTOR_DATA_FILE_SUFFIX;

    if(!file_exists(indexFilename)) {
        errorLog <<"Index file not exists: " <<  indexFilename << endl << errorExit;
    }

    data_filename = extract_base_file_name(filename) + FILEVECTOR_DATA_FILE_SUFFIX;
    if(!file_exists(data_filename))
        errorLog <<"Data file not exists: " <<  data_filename.c_str() << endl <<errorExit;

    struct stat data_filestatus;
    stat(data_filename.c_str(), &data_filestatus);

    struct stat index_filestatus;
    stat(indexFilename.c_str(), &index_filestatus);

    indexFile.open(indexFilename.c_str(), ios::out | ios::in | ios::binary);
    if (!indexFile) {
        errorLog << "Opening file "<< indexFilename <<" for write & read failed\n" << errorExit;
    }

    dataFile.open(data_filename.c_str(), ios::out | ios::in | ios::binary);
    if (!dataFile) {
        errorLog << "Opening file "<< data_filename << " for write & read failed\n" << errorExit;
    }

	indexFile.read((char*)&fileHeader, sizeof(fileHeader));
	if (!indexFile){
        errorLog << "Failed to read datainfo from file:" << indexFilename << endl;
    }

    // some integrity checks
	if (getElementSize() != fileHeader.bytesPerRecord) {
		errorLog << "System data type size ("<<getElementSize();
		errorLog <<") and file data type size ("<<fileHeader.bytesPerRecord<<") do not match.\n";
	}

	//!!! nelements should actually be long to ensure !!!
	if (fileHeader.nelements != (fileHeader.numObservations*fileHeader.numVariables)) {
		errorLog << "Number of variables ("<<fileHeader.numVariables;
		errorLog << ") and observations ("<<fileHeader.numObservations<<") do not multiply to nelements";
		errorLog << "("<< fileHeader.nelements<<") (file integrity issue?)\n";
		errorLog << errorExit;
	}

	if ((fileHeader.bytesPerRecord != (fileHeader.bitsPerRecord/8)) ||
	     ((fileHeader.bitsPerRecord % 8) != 0) || (fileHeader.bitsPerRecord < 8)) {
		errorLog << "Size in bytes/bits do not match or bit-size of char !=8 or ";
		errorLog << "non-byte recods (file integrity issue?)" << errorExit;
	}

  	unsigned long indexSize = sizeof(fileHeader) + sizeof(fixedchar)*(fileHeader.numVariables+fileHeader.numObservations);
    if(indexSize != index_filestatus.st_size) {
        errorLog << "Index file "<<indexFilename<<" size(" << index_filestatus.st_size << ") differs from the expected(";
        errorLog << indexSize <<")" << endl << errorExit;
    }

	// temp fix because nelements is not yet long ... !!!
    //	unsigned long estimated_size = data_type.bytes_per_record*data_type.nelements + headerSize;
	unsigned long estimated_size =
			(unsigned long) fileHeader.bytesPerRecord *
			(unsigned long) fileHeader.numVariables *
			(unsigned long) fileHeader.numObservations;

	if (estimated_size != data_filestatus.st_size) {
        errorLog << "Data file size (" << data_filestatus.st_size;
        errorLog << ") differs from the expected ("<<estimated_size<<")"<<endl<<" [";
        errorLog << fileHeader.numVariables<<","<<fileHeader.numObservations<<"]" << endl;
        errorLog << errorExit;
    }

    variableNames = 0;
    observationNames = 0;

    setCacheSizeInMb(cachesizeMb);
    update_cache(0);
    dbg << "Filevector " << filename << " opened." << endl;
}

// read in variable and observation names
void FileVector::readNames() {
    if (variableNames) delete[]variableNames;
    if (observationNames) delete[]observationNames;

	variableNames = new (nothrow) fixedchar [fileHeader.numVariables];
	if (!variableNames) errorLog << "can not get RAM for variable names" << errorExit;
	observationNames = new (nothrow) fixedchar [fileHeader.numObservations];
	if (!observationNames) errorLog << "can not get RAM for observation names" << errorExit;

	indexFile.seekg(sizeof(fileHeader), ios::beg);
	for (unsigned long i=0;i<fileHeader.numObservations;i++)
    	indexFile.read((char*)(observationNames+i),sizeof(fixedchar));
	for (unsigned long i=0;i<fileHeader.numVariables;i++)
		indexFile.read((char*)(variableNames+i),sizeof(fixedchar));
}

unsigned long FileVector::getCacheSizeInMb() {
	return cache_size_Mb;
}

void FileVector::setCacheSizeInMb( unsigned long cachesizeMb ) {
// figure out cache size
	cache_size_Mb = cachesizeMb;
	cache_size_nvars = (unsigned long) 1024*1024*cache_size_Mb/(fileHeader.numObservations*fileHeader.bytesPerRecord);
	if (cache_size_nvars<1) {
		cache_size_Mb = (long unsigned int) ceil(
				(float) fileHeader.numObservations*fileHeader.bytesPerRecord/(1024.*1024.)
				);
		cache_size_nvars = 1;
	} else if (cache_size_nvars>fileHeader.numVariables) {
		cache_size_Mb = (long unsigned int) ceil(
				(float) fileHeader.numVariables*fileHeader.numObservations*fileHeader.bytesPerRecord/(1024.*1024.)
				);
		cache_size_nvars = fileHeader.numVariables;
	}
	cache_size_bytes = cache_size_nvars*fileHeader.bytesPerRecord*fileHeader.numObservations*sizeof(char);

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

void FileVector::update_cache(unsigned long from_var) {
	unsigned long current_cache_size_bytes = cache_size_bytes;
	in_cache_from = from_var;
	in_cache_to = from_var + cache_size_nvars - 1;
	if (in_cache_to >= fileHeader.numVariables) {
		in_cache_to = fileHeader.numVariables-1;
		current_cache_size_bytes = (in_cache_to-in_cache_from+1)*
					   fileHeader.bytesPerRecord*fileHeader.numObservations*sizeof(char);
	}
	deepDbg << "Updating cache: " << in_cache_from << " - " << in_cache_to << "\n";
	unsigned long internal_from = in_cache_from*fileHeader.numObservations*fileHeader.bytesPerRecord*sizeof(char);
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

void FileVector::setUpdateNamesOnWrite(bool bUpdate) {
    updateNamesOnWrite = bUpdate;
}

void FileVector::cacheAllNames(bool doCache) {
    if (doCache) {
        if (variableNames || observationNames) {
            dbg << "FileVector.cacheAllNames(true) called while variable names are already cached." << endl;
            return;
        }
        readNames();
    } else {
        if (variableNames) {
            delete[] variableNames;
            variableNames = 0;
        }
        if (observationNames) {
            delete [] observationNames;
            observationNames = 0;
        }
    }
}

void FileVector::writeVariableName(unsigned long varIdx, fixedchar name) {
	if (varIdx >= fileHeader.numVariables) {
	    errorLog << "Trying to set name of obs out of range (" << varIdx << ")\n\n" << endl << errorExit;
	}
	if (updateNamesOnWrite||variableNames == 0){
	    indexFile.seekp(sizeof(fileHeader) + sizeof(fixedchar)*(varIdx + fileHeader.numObservations), ios::beg);
	    indexFile.write((char*)&name, sizeof(fixedchar));
	}
	if (variableNames) {
	    variableNames[varIdx] = name;
	}
}

void FileVector::writeObservationName(unsigned long obsIdx, fixedchar name) {
	if (obsIdx >= fileHeader.numObservations) {
	    errorLog << "Trying to set name of vars out of range (" << obsIdx << ")\n\n" << endl << errorExit;
	}
	if (updateNamesOnWrite || observationNames == 0){
	    indexFile.seekp(sizeof(fileHeader) + sizeof(fixedchar)*(obsIdx), ios::beg);
	    indexFile.write((char*)&name, sizeof(fixedchar));
    }
    if (observationNames) {
        observationNames[obsIdx] = name;
    }
}

fixedchar FileVector::readVariableName(unsigned long varIdx) {
	if (varIdx>=fileHeader.numVariables) {
	    errorLog << "trying to get name of var out of range" << errorExit;
	}

	if (!variableNames) {
	    fixedchar ret;
	    indexFile.seekp(sizeof(fileHeader) + sizeof(fixedchar)*(varIdx+fileHeader.numObservations), ios::beg);
	    indexFile.read((char*)&ret, sizeof(fixedchar));
	    return ret;
	}

	return variableNames[varIdx];
}

fixedchar FileVector::readObservationName(unsigned long obsIdx) {
	if (obsIdx >= fileHeader.numObservations) {
	    errorLog << "trying to get name of obs out of range" << errorExit;
	}

	if (!observationNames) {
	    fixedchar ret;
	    indexFile.seekp(sizeof(fileHeader) + sizeof(fixedchar)*(obsIdx), ios::beg);
	    indexFile.read((char*)&ret, sizeof(fixedchar));
	    return ret;
	}

	return observationNames[obsIdx];
}

// can read single variable
void FileVector::readVariable(unsigned long nvar, void * outvec) {
	if (nvar>=fileHeader.numVariables) {
	    errorLog << "Variable number out of range (" << nvar << " >= " << fileHeader.numVariables <<")"<<endl << errorExit;
	}
	if (in_cache_to > 0 && nvar >= in_cache_from && nvar <= in_cache_to) {
		unsigned long offset = (nvar - in_cache_from)*fileHeader.numObservations*getElementSize();
		memcpy(outvec,cached_data+offset,getElementSize()*(fileHeader.numObservations));
    } else {
		update_cache(nvar);
        memcpy(outvec,cached_data,getElementSize()*fileHeader.numObservations);
	}
}

void FileVector::readObservation(unsigned long obsIdx, void *outvec) {
	char * tmpdata = new (nothrow) char [getNumObservations()*getElementSize()];
	if(!tmpdata)
		errorLog << "readObservation: cannot allocate tmpdata" << errorExit;

	for( int i = 0; i< getNumVariables(); i++)
	{
		readVariable(i, tmpdata);
		memcpy((char*)outvec+i*getElementSize(),tmpdata+getElementSize()*obsIdx,getElementSize());
	}
	delete[] tmpdata;
}

void FileVector::writeObservation(unsigned long obsIdx, void * invec) {
    if (readOnly) {
        errorLog << "Trying to write to the readonly file." << errorExit;
    }
	for( int i = 0; i< getNumVariables(); i++)
	{
		writeElement( i, obsIdx, (char*)invec+ i*getElementSize() );
	}
}

// can write single variable
void FileVector::writeVariable(unsigned long nvar, void * datavec) {
    if (readOnly) {
        errorLog << "Trying to write to the readonly file." << errorExit;
    }
	unsigned long pos = nrnc_to_nelem(nvar, 0);
	dataFile.seekp(pos*getElementSize(), ios::beg);
	dataFile.write((char*)datavec,getElementSize()*fileHeader.numObservations);
	if (!dataFile) {
	    errorLog <<"failed to write to data file\n"<<errorExit;
	}

	//update data in cache
// 	deepDbg << "var:"<< nvar << ",cache from :"<< in_cache_from << ", to: "<< in_cache_to  << endl;

	if (nvar >= in_cache_from && nvar <= in_cache_to)
	{
	    unsigned long offset = (nvar - in_cache_from)*fileHeader.numObservations*getElementSize();
 	    memcpy(cached_data + offset,datavec,getElementSize()*fileHeader.numObservations);
	}
}

unsigned long FileVector::nrnc_to_nelem(unsigned long nvar, unsigned long obsIdx) {
    if (nvar >= fileHeader.numVariables) {
        errorLog << "Variable number out of bounds (" << nvar << " >= " <<  fileHeader.numVariables << ")" << endl << errorExit;
    }
    if (obsIdx >= fileHeader.numObservations) {
        errorLog << "Observation number out of bounds (" << obsIdx << " >= " <<  fileHeader.numVariables << ")" << endl << errorExit;
    }
    return( nvar * fileHeader.numObservations + obsIdx );
}

// should only be used for reading single random elements!

void FileVector::readElement(unsigned long nvar, unsigned long obsIdx, void* out) {
    deepDbg << "FileVector.readElement(" << nvar << "," << obsIdx << ");" << endl;
    unsigned long pos = nrnc_to_nelem(nvar, obsIdx);
    dataFile.seekg(pos*getElementSize(), ios::beg);
    dataFile.read((char*)out,getElementSize());
}

void FileVector::writeElement(unsigned long nvar, unsigned long obsIdx, void* data) {
    if (readOnly) {
        errorLog << "Trying to write to the readonly file." << errorExit;
    }
    deepDbg << "FileVector.writeElement(" << nvar << "," << obsIdx << ");" << endl;
    unsigned long pos = nrnc_to_nelem(nvar, obsIdx);
    dataFile.seekp(pos*getElementSize(), ios::beg);
    dataFile.write((char*)data,getElementSize());
    dataFile.flush();

    if (nvar >= in_cache_from && nvar <= in_cache_to) {
	    unsigned long offset = (nvar - in_cache_from)*fileHeader.numObservations*getElementSize() + obsIdx *getElementSize();
	    memcpy(cached_data+offset,data,getElementSize() );
    }
}

unsigned int FileVector::getNumVariables() {
   return fileHeader.numVariables;
}

unsigned int FileVector::getNumObservations() {
   return fileHeader.numObservations;
}

void FileVector::saveAs( string newFilename ) {
    initializeEmptyFile( (char *)newFilename.c_str(), getNumVariables(), getNumObservations(), fileHeader.type, true);
    FileVector *outdata = new FileVector( newFilename, 64 );//todo which size for cache to use?

    // copy observation names from the first object
  	for (unsigned long i=0;i<getNumObservations();i++)
  	    outdata->writeObservationName( i, readObservationName( i ) );

    char * tmpvariable = new (nothrow) char[getNumObservations()*getElementSize()];
    if (!tmpvariable) errorLog << "can not allocate memory for tmpvariable" << endl << endl << errorExit;

    for (unsigned long i=0 ; i<getNumVariables();i++)
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

void FileVector::saveVariablesAs( string newFilename, unsigned long nvars, unsigned long *varindexes) {
    initializeEmptyFile( (char *)newFilename.c_str(), nvars, getNumObservations(), fileHeader.type, true);
    FileVector outdata( newFilename, 64 );//todo which size for cache to use?

    // copy observation names from the first object
  	for (unsigned long i=0;i<getNumObservations();i++)
  	    outdata.writeObservationName( i, readObservationName( i ) );

    char * tmpvariable = new (nothrow) char[getNumObservations()*getElementSize()];
    if (!tmpvariable) errorLog << "can not allocate memory for tmpvariable" << endl << endl << errorExit;

    for ( unsigned long i=0 ; i<nvars ; i++ )
    {
        unsigned long selected_index =  varindexes[i];
        //write var names
        outdata.writeVariableName( i, readVariableName(selected_index ));
        //write variables
        readVariable(selected_index,tmpvariable);
        outdata.writeVariable(i,tmpvariable);
    }

    delete [] tmpvariable;
}

void FileVector::saveObservationsAs( string newFilename, unsigned long nobss, unsigned long *obsindexes) {
    if (headerOrDataExists(newFilename))
    {
        errorLog << "File " << newFilename <<" already exists" << endl << errorExit;
    }
    initializeEmptyFile( (char *)newFilename.c_str(), getNumVariables(), nobss, fileHeader.type,true);
    FileVector outdata( newFilename, 64 );//todo which size for cache to use?

    // copy observation names from the first object
  	for( unsigned long i=0 ; i < nobss ; i++ )
  	    outdata.writeObservationName( i, readObservationName( obsindexes[i] ) );

    char * in_variable = new (nothrow) char[getNumObservations()*getElementSize()];
    if (!in_variable) errorLog << "can not allocate memory for tmpvariable" << endl << endl << errorExit;

    char * out_variable = new (nothrow) char[nobss*getElementSize()];
    if (!out_variable) errorLog << "can not allocate memory for tmpvariable" << endl << endl << errorExit;
    for ( unsigned long i=0 ; i<getNumVariables() ; i++ )
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
void FileVector::copyVariable(char* to, char* from, int n, unsigned long * indexes ) {
	for ( int j=0 ; j<n ; j++ )	{
		//copy only selected observations to out_variable  from in_variable
		int read_offset = indexes[j]*getElementSize();
		if(read_offset + getElementSize() > getNumObservations() * getElementSize()) {
		    errorLog << "When saving selected observations: index in obsindexes(" <<indexes[j];
		    errorLog << ") is out of range, source obsIdx is " << getNumObservations()<< endl;
		    errorLog << errorExit;
		}
		memcpy(to + j*getElementSize(),from + read_offset,getElementSize());
	}
}

void FileVector::saveAs(string newFilename, unsigned long nvars, unsigned long nobss, unsigned long *varindexes, unsigned long *obsindexes) {
    if (headerOrDataExists(newFilename)) {
        errorLog << "File "<< newFilename <<" already exists." << endl;
    }
    initializeEmptyFile( (char *)newFilename.c_str(), nvars, nobss, fileHeader.type, true);
    FileVector outdata( newFilename, 64 );//todo which size for cache to use?

    // copy observation names from the first object
	for( unsigned long i=0 ; i < nobss ; i++ )
		outdata.writeObservationName( i, readObservationName( obsindexes[i] ) );

    char * out_variable = new (nothrow) char[nobss*getElementSize()];
    if (!out_variable) errorLog << "can not allocate memory for out_variable" << errorExit;

	char * in_variable = new (nothrow) char[getNumObservations()*getElementSize()];
	if (!in_variable) errorLog << "can not allocate memory for in_variable" << endl << endl << errorExit;

	for( unsigned long i=0 ; i<nvars ; i++ ) {
		unsigned long selected_index =  varindexes[i];
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

void FileVector::saveAsText(string newFilename, unsigned long nvars, unsigned long nobss,
    unsigned long *varindexes, unsigned long *obsindexes) {

    dbg << "nvars = " << nvars << endl;
    dbg << "obsIdx = " << nobss << endl;

    ofstream textfile(newFilename.c_str(), ios::out);

    // copy observation names from the first object
	for( unsigned long i=0 ; i < nobss ; i++ ) {
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

	for(unsigned long i=0; i<nvars; i++) {
        dbg << "Writing var " << i << " of " << nvars << endl;
		unsigned long selected_index = varindexes[i];
    	//write var names
    	fixedchar fc = readVariableName(selected_index);
		textfile << fc.name << " ";
		//write variables
		readVariable(selected_index, in_variable);
		copyVariable(out_variable, in_variable, nobss, obsindexes);

	    for(unsigned long j=0; j<nobss; j++) {
	        double x;
	        performCast(x, &out_variable[j*getElementSize()],getElementType());
		    textfile << x << " ";
		}
		textfile << endl;
	}

	delete[] in_variable;
	delete[] out_variable;
}

short unsigned FileVector::getElementSize() {
    return calcDataSize(fileHeader.type);
}

short unsigned FileVector::getElementType() {
    return fileHeader.type;
}

void FileVector::addVariable(void *invec, string varName) {
    deepDbg << "addVariable(" << varName << ")" << endl;
    if (readOnly) {
        errorLog << "Trying to write to the readonly file." << errorExit;
    }

    fileHeader.numVariables++;
    //recalculate
    fileHeader.nelements = fileHeader.numVariables*fileHeader.numObservations;

    fixedchar _fc_varname(varName);

    // are names loaded from disk ?
    if (variableNames && observationNames) {

        fixedchar * newVariablesNames = new (nothrow)fixedchar[fileHeader.numVariables];
        if (!newVariablesNames) {
            errorLog << "Can not allocate memory in addVariable()" << errorExit;
        }

        //reallocate greater array for var names
        memcpy(newVariablesNames, variableNames, sizeof(fixedchar)*(fileHeader.numVariables-1));
        newVariablesNames[fileHeader.numVariables - 1] = _fc_varname;
        fixedchar *oldvar_names = variableNames;
        variableNames = newVariablesNames;
        delete[] oldvar_names;
        if (updateNamesOnWrite) {
	        indexFile.seekp(sizeof(fileHeader) + sizeof(fixedchar)*(fileHeader.numVariables - 1 + fileHeader.numObservations), ios::beg);
	        indexFile.write((char*)&_fc_varname.name, sizeof(fixedchar));
	    }
    } else { // not loaded
	    indexFile.seekp(sizeof(fileHeader) + sizeof(fixedchar)*(fileHeader.numVariables - 1 + fileHeader.numObservations), ios::beg);
	    indexFile.write((char*)&_fc_varname.name, sizeof(fixedchar));
    }
    writeVariable(fileHeader.numVariables - 1, invec);
}
