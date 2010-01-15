#include <sys/stat.h>
#include <string.h>
#include <iostream>

using namespace std;

#include "filevector.h"
#include "frutil.h"

void filevector::free_resources()
{
	if (connected) {
	    index_file.seekp(0, ios::beg);
	    index_file.write((char*)&data_type, sizeof(data_type));
		index_file.seekp(sizeof(data_type), ios::beg);
// may be have to fix: is the buffer always enough???
		if (sizeof(fixedchar)*data_type.nobservations > INT_MAX) error("sizeof(fixedchar)*data_type.nobservations > INT_MAX\n\n");
		index_file.write((char*)observation_names,sizeof(fixedchar)*data_type.nobservations);
		index_file.seekp(sizeof(data_type)+sizeof(fixedchar)*data_type.nobservations, ios::beg);
// may be have to fix: is the buffer always enough???
		if (sizeof(fixedchar)*data_type.nvariables > INT_MAX) error("sizeof(fixedchar)*data_type.nvariables > INT_MAX\n\n");
		index_file.write((char*)variable_names,sizeof(fixedchar)*data_type.nvariables);
		delete [] char_buffer;
		delete [] observation_names;
		delete [] variable_names;
//		cout << "!!! destr + free !!!\n";
	}
	connected = 0;
	index_file.close();
	data_file.close();
//	cout << "!!! destr !!!\n";
}

filevector::~filevector()
{
    free_resources();
}

void filevector::initialize(string filename_toload, unsigned long int cachesizeMb)
{
	if (sizeof(unsigned long int) != 8) warning("you appear to work on 32-bit system... large files not supported\n");

    index_filename = extract_base_file_name(filename_toload) + FILEVECTOR_INDEX_FILE_SUFFIX;

    if(!file_exists(index_filename))
          error("index file not exists: %s",index_filename.c_str());

   data_filename = extract_base_file_name(filename_toload)+ FILEVECTOR_DATA_FILE_SUFFIX;
    if(!file_exists(data_filename))
         error("data file not exists: %s",data_filename.c_str());



	if (connected) error("trying to ini already ini-ed object!\n\n");

	struct stat index_filestatus;
	int res = stat( index_filename.c_str() , &index_filestatus);
	cout << "stat (" + index_filename + ") = " << res << endl;
	cout << "index_filestatus.st_size = " << index_filestatus.st_size << endl;
	cout << "sizeof(data_type) = " << sizeof(data_type) << endl;
    if (index_filestatus.st_size < sizeof(data_type))
           error("index file %s is too short to contain an FV-index\n",index_filename.c_str());

	struct stat data_filestatus;
	stat( data_filename.c_str() , &data_filestatus);

//	cout << "1" << endl;

	index_file.open(index_filename.c_str(), ios::out | ios::in | ios::binary);
	if (!index_file)
		error("opening file %s for write & read failed\n",index_filename.c_str());

//	cout << "2" << endl;

	data_file.open(data_filename.c_str(), ios::out | ios::in | ios::binary);
	if (!data_file)
		error("opening file %s for write & read failed\n",data_filename.c_str());

//	cout << "3" << endl;

	index_file.read((char*)&data_type,sizeof(data_type));
	if (!index_file)
        error("failed to read datainfo from file '%s'\n",index_filename.c_str());

//	cout << "4" << endl;

//    // some integrity checks
//	if (getDataSize() != data_type.bytes_per_record)
//		error("system data type size (%d) and file data type size (%d) do not match\n",
//			getDataSize(),data_type.bytes_per_record);
	//!!! nelements should actually be long to ensure !!!
	if (data_type.nelements != (data_type.nobservations*data_type.nvariables))
		error("number of variables (%lu) and observations (%lu) do not multiply to nelements (%lu) (file integrity issue?)\n",
			data_type.nvariables, data_type.nobservations, data_type.nelements);
	if ((data_type.bytes_per_record != (data_type.bits_per_record/8)) ||
	     ((data_type.bits_per_record % 8) != 0) || (data_type.bits_per_record < 8))
		perror("size in bytes/bits do not match or bit-size of char !=8 or non-byte recods (file integrity issue?)");

// reserved for future use -- if bits storage ever used ...
//	unsigned long int extrabits = 0;
//	unsigned long int estimated_size = data_type.bits_per_record*data_type.nelements;
//	unsigned long int resid = (estimated_size % 8);
//	if (resid != 0) estimated_size += (8 - resid);
//	estimated_size /= 8;

//	cout << "5" << endl;

	header_size = sizeof(data_type) + sizeof(fixedchar)*(data_type.nvariables+data_type.nobservations);
    if(header_size != index_filestatus.st_size)
        error("index file size(%lu) differs from the expected(%lu)",index_filestatus.st_size,header_size );

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

//	cout << "6" << endl;

    set_cachesizeMb(cachesizeMb);
    update_cache(0);
	connected = 1;
}

unsigned long int filevector::get_cachesizeMb()
{
	return(cache_size_Mb);
}

void filevector::set_cachesizeMb( unsigned long int cachesizeMb )
{
// figure out cache size
	cache_size_Mb = cachesizeMb;
	cache_size_nvars = (unsigned long int) 1024*1024*cache_size_Mb/(data_type.nobservations*data_type.bytes_per_record);
	if (cache_size_nvars<1) {
		message("attempting to set cache size to 1 var (%f Mb)\n",
		         (float) data_type.nobservations*data_type.bytes_per_record/(1024.*1024.));
		cache_size_Mb = ceil((float) data_type.nobservations*data_type.bytes_per_record/(1024.*1024.));
		cache_size_nvars = 1;
	} else if (cache_size_nvars>data_type.nvariables) {
		message("attempting to cache all the data (%u variables, %f Mb)\n",
			 data_type.nvariables,
		         (float) data_type.nvariables*data_type.nobservations*data_type.bytes_per_record/(1024.*1024.));
		cache_size_Mb = ceil((float) data_type.nvariables*data_type.nobservations*data_type.bytes_per_record/(1024.*1024.));
		cache_size_nvars = data_type.nvariables;
	} else {
		message("attempting to cache specified amount of data (%u variables, %f Mb)\n",
			 cache_size_nvars,
		         (float) cache_size_nvars*data_type.nobservations*data_type.bytes_per_record/(1024.*1024.));
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
//	cout << "updating cache: " << in_cache_from << " - " << in_cache_to << "\n";
	unsigned long int internal_from = in_cache_from*data_type.nobservations*data_type.bytes_per_record*sizeof(char);
//	cout << "position = " << internal_from << "\n";
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
//TMP
//	for (int i=0;i<cache_size_nvars;i++) {
//	for (int j=0;j<data_type.nobservations;j++)
//	cout << " " << cached_data[i*data_type.nobservations+j];
//	cout<<"\n";
//	}
}


void filevector::write_variable_name(unsigned long int nvar, fixedchar name)
{
	if (nvar>=data_type.nvariables) error("trying to set name of obs out of range (%lu)\n\n",nvar);
	variable_names[nvar] = name;
}


void filevector::write_observation_name(unsigned long int nobs, fixedchar name)
{
	if (nobs>=data_type.nobservations) error("trying to set name of vars out of range (%lu)\n\n",nobs);
	observation_names[nobs] = name;
}



fixedchar filevector::read_variable_name(unsigned long int nvar)
{
	if (nvar>=data_type.nvariables) error("trying to get name of var out of range");
	return(variable_names[nvar]);
}


fixedchar filevector::read_observation_name(unsigned long int nobs)
{
	if (nobs>=data_type.nobservations) error("trying to get name of obs out of range");
	return(observation_names[nobs]);
}

// can read single variable

void filevector::read_variable(unsigned long int nvar, void * outvec)
{
	if (nvar>=data_type.nvariables) error("nvar out of range (%lu >= %lu)",nvar,data_type.nvariables);
	if (in_cache_to > 0 && nvar >= in_cache_from && nvar <= in_cache_to) {
		unsigned long int offset = (nvar - in_cache_from)*data_type.nobservations*getDataSize();
		memcpy(outvec,cached_data+offset,getDataSize()*(data_type.nobservations));
    } else {
		update_cache(nvar);
        memcpy(outvec,cached_data,getDataSize()*data_type.nobservations);
	}
}

void filevector::read_observation(unsigned long int nobs, void * outvec)
{
	char * tmpdata = new (nothrow) char [get_nobservations()*getDataSize()];
	if(!tmpdata)
		error("read_observation: cannot allocate tmpdata");

	for( int i = 0; i< get_nvariables(); i++)
	{
		read_variable(i, tmpdata);
//		outvec[i] = tmpdata[nobs];
		memcpy((char*)outvec+i*getDataSize(),tmpdata+getDataSize()*i,getDataSize());
	}
	delete[] tmpdata;
}


void filevector::write_observation(unsigned long int nobs, void * invec)
{
	for( int i = 0; i< get_nvariables(); i++)
	{
		write_element( i, nobs, (char*)invec+ i*getDataSize() );
	}
}

// can write single variable

void filevector::write_variable(unsigned long int nvar, void * datavec)
{
	unsigned long int pos = nrnc_to_nelem(nvar, 0);
	data_file.seekp(pos*getDataSize(), ios::beg);
	data_file.write((char*)datavec,getDataSize()*data_type.nobservations);
	if (!data_file) error ("failed to write to data file\n");

	//update data in cache
//	cout << "var:"<< nvar << ",cache from :"<< in_cache_from << ", to: "<< in_cache_to  << endl;

	if (nvar >= in_cache_from && nvar <= in_cache_to)
	{
//	    cout<< "updating data in cache" << endl;
	    unsigned long int offset = (nvar - in_cache_from)*data_type.nobservations;
//		for (unsigned long int i = 0;i<data_type.nobservations;i++)
//		{
		    //cached_data[offset+i]= datavec[i];
//        }
 	    memcpy(cached_data+offset,datavec,getDataSize()*data_type.nobservations);
	}

//TMP
//	for (unsigned int i=0;i<data_type.nobservations;i++) cout << " " << datavec[i];
}



unsigned long int filevector::nrnc_to_nelem(unsigned long int nvar, unsigned long int nobs)
{
	if (nvar >= data_type.nvariables || nobs >= data_type.nobservations)
		error("nvar >= real or nobs >= real (%u >= %u || %u >= %u)\n",
			nvar,data_type.nvariables,nobs,data_type.nobservations);
	return( nvar * data_type.nobservations + nobs );
}

// should only be used for reading single random elements!

void filevector::read_element(unsigned long int nvar, unsigned long int nobs, void * out)
{
    //todo use cache
	unsigned long int pos = nrnc_to_nelem(nvar, nobs);
	data_file.seekg(pos*getDataSize(), ios::beg);
	data_file.read((char*)&out,getDataSize());
	if (!data_file) error("failed to read an element from file '%s'\n",data_filename.c_str());
}


void filevector::write_element(unsigned long int nvar, unsigned long int nobs, void* data)
{
	unsigned long int pos = nrnc_to_nelem(nvar, nobs);
	data_file.seekp(pos*getDataSize(), ios::beg);
	data_file.write((char*)data,getDataSize());

	if (nvar >= in_cache_from && nvar <= in_cache_to)
	{
		unsigned long int offset = (nvar - in_cache_from)*data_type.nobservations;
		memcpy(cached_data+(offset+nobs)*getDataSize(),data,getDataSize() );
		//cached_data[offset+nobs]= data;
	}
}


unsigned int filevector::get_nvariables()
{
   if(!connected)
   {
       error("cannot return nvariables, not connected\n");
   }
   return data_type.nvariables;
}



unsigned int filevector::get_nobservations()
{
   if(!connected)
   {
       error("cannot return nobservations, not connected\n");
   }
   return data_type.nobservations;
}


void filevector::save( string new_file_name )
{
    initialize_empty_file( (char *)new_file_name.c_str(), get_nvariables(), get_nobservations(), data_type.type);
    filevector *outdata = new filevector( new_file_name, 64 );//todo which size for cache to use?

    // copy observation names from the first object
  	for (unsigned long int i=0;i<get_nobservations();i++)
  	    outdata->write_observation_name( i, read_observation_name( i ) );

    char * tmpvariable = new (nothrow) char[get_nobservations()*getDataSize()];
    if (!tmpvariable) error("can not allocate memory for tmpvariable\n\n");

    for (unsigned long int i=0 ; i<get_nvariables();i++)
    {
        //write var names
        outdata->write_variable_name( i, read_variable_name(i));
        //write variables
        read_variable(i,tmpvariable);
        outdata->write_variable(i,tmpvariable);
    }
    delete outdata;
    delete [] tmpvariable;
}



void filevector::save_vars( string new_file_name, unsigned long int nvars, unsigned long int * varindexes)
{
    initialize_empty_file( (char *)new_file_name.c_str(), nvars, get_nobservations(), data_type.type);
    filevector outdata( new_file_name, 64 );//todo which size for cache to use?

    // copy observation names from the first object
  	for (unsigned long int i=0;i<get_nobservations();i++)
  	    outdata.write_observation_name( i, read_observation_name( i ) );

    char * tmpvariable = new (nothrow) char[get_nobservations()*getDataSize()];
    if (!tmpvariable) error("can not allocate memory for tmpvariable\n\n");

    for ( unsigned long int i=0 ; i<nvars ; i++ )
    {
        unsigned long int selected_index =  varindexes[i];
        //write var names
        outdata.write_variable_name( i, read_variable_name(selected_index ));
        //write variables
        read_variable(selected_index,tmpvariable);
        outdata.write_variable(i,tmpvariable);
    }

    delete [] tmpvariable;
}

void filevector::save_obs( string new_file_name, unsigned long int nobss, unsigned long int * obsindexes)
{
    initialize_empty_file( (char *)new_file_name.c_str(), get_nvariables(), nobss, data_type.type);
    filevector outdata( new_file_name, 64 );//todo which size for cache to use?

    // copy observation names from the first object
  	for( unsigned long int i=0 ; i < nobss ; i++ )
  	    outdata.write_observation_name( i, read_observation_name( obsindexes[i] ) );


    char * in_variable = new (nothrow) char[get_nobservations()*getDataSize()];
    if (!in_variable) error("can not allocate memory for tmpvariable\n\n");

    char * out_variable = new (nothrow) char[nobss*getDataSize()];
    if (!out_variable) error("can not allocate memory for tmpvariable\n\n");
    for ( unsigned long int i=0 ; i<get_nvariables() ; i++ )
    {
        //write var names
        outdata.write_variable_name( i, read_variable_name(i));
        //write variables
        read_variable(i, in_variable);
        memcpy(out_variable,in_variable,getDataSize()*nobss);
        //copy data to reduced struct for writing
//        for ( unsigned long int j=0 ; j<nobss ; j++ )
//        {
//            out_variable[j] = in_variable[obsindexes[j]];
//        }
        outdata.write_variable(i,out_variable);
    }

    delete [] in_variable;
    delete [] out_variable;
}

short unsigned filevector::getDataSize(){
    return calcDataSize(data_type.type);
}

short unsigned filevector::getDataType(){
    return data_type.type;


}


void filevector::add_variable(void * invec, string varname)
{
      data_type.nvariables++;
      data_type.nelements = data_type.nvariables*data_type.nobservations;//recalculate

      fixedchar * new_variable_names = new (nothrow)fixedchar[data_type.nvariables];
      if (!new_variable_names)
          error("can not allocate memory in add_variable()");

	  //reallocate greater array for var names
	  memcpy(new_variable_names,variable_names,sizeof(fixedchar)*(data_type.nvariables-1));
      fixedchar _fc_varname(varname);
      new_variable_names[data_type.nvariables - 1] = _fc_varname;
      fixedchar * oldvar_names = variable_names;
      variable_names = new_variable_names;
      delete[] oldvar_names;

      write_variable(data_type.nvariables - 1,invec);
}



