#ifndef __FRVECTOR__
#define __FRVECTOR__

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <climits>
#include <new>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>

#include "frutil.h"
#include "frerror.h"

#include "DatABELBaseCPP.h"

using namespace std;

template <class DT> class filevector: public DatABELBaseCPP<DT>
{
public:
	string filename;
	std::fstream data_file;
	fr_type data_type;
// row and column names
	fixedchar * variable_names;
	fixedchar * observation_names;
// size of header (descriptives + var/obs names)
	unsigned long int header_size;
// cache size (Mb) requested by user
	unsigned long int cache_size_Mb;
// if the objct is connected to file
	short int connected;
// cache size internal; these ones are exact and used internaly
	unsigned long int cache_size_nvars;
	unsigned long int cache_size_bytes;
	unsigned long int max_buffer_size_bytes;
// which variables are now in cache
	unsigned long int in_cache_from;
	unsigned long int in_cache_to;
	DT * cached_data;
	char * char_buffer;

// prototypes
	filevector();
	~filevector();

// constructor based on initialize
	filevector(string filename_toload, unsigned long int cachesizeMb);

// these ones are the actual used to initialize and free up
	void initialize(string filename_toload, unsigned long int cachesizeMb);
	void free_resources();
// this one updates cache
	void update_cache(unsigned long int from_var);
// gives element number from nvar & nobs
	unsigned long int nrnc_to_nelem(unsigned long int nvar, unsigned long int nobs);

// getting and setting var/col names
	void write_variable_name(unsigned long int nvar, fixedchar name);
	void write_observation_name(unsigned long int nobs, fixedchar name);

	unsigned int get_nvariables();
    unsigned int get_nobservations();

	fixedchar read_variable_name(unsigned long int nvar);
	fixedchar read_observation_name(unsigned long int nobs);

// USER FUNCTIONS
// can read single variable
	void read_variable(unsigned long int nvar, DT * outvec);

// should only be used for reading single random elements!
	DT read_element(unsigned long int nvar, unsigned long int nobs);
// write single variable
	void write_variable(unsigned long int nvar, DT * datavec);
//	void add_variable(DT * invec, fixedchar varname);
// write single element
	void write_element(unsigned long int nvar, unsigned long int nobs, DT data);

	void read_observation(unsigned long int nobs, DT * outvec);
	void write_observation(unsigned long int nobs, DT * outvec);

	void save( string new_file_name );
	void save_vars( string new_file_name, unsigned long int nvars, unsigned long int * varindexes);
	void save_obs( string new_file_name, unsigned long int nobss, unsigned long int * obsindexes);

	void set_cachesizeMb( unsigned long int cachesizeMb );

// FOR FUTURE:
// very slow one!
//	DT * read_observation(unsigned long int nobs);
// should only be used for reading single random elements!
//	DT read_element(unsigned long int nelment);

};


//template <class DT>
//filevector<DT>::~filevector()
//{
	//free_resources();
//}

template <class DT>
void filevector<DT>::free_resources()
{
	if (connected) {
		data_file.seekp(sizeof(data_type), std::ios::beg);
// may be have to fix: is the buffer always enough???
		if (sizeof(fixedchar)*data_type.nobservations > INT_MAX) error("sizeof(fixedchar)*data_type.nobservations > INT_MAX\n\n");
		data_file.write((char*)observation_names,sizeof(fixedchar)*data_type.nobservations);
		data_file.seekp(sizeof(data_type)+sizeof(fixedchar)*data_type.nobservations, std::ios::beg);
// may be have to fix: is the buffer always enough???
		if (sizeof(fixedchar)*data_type.nvariables > INT_MAX) error("sizeof(fixedchar)*data_type.nvariables > INT_MAX\n\n");
		data_file.write((char*)variable_names,sizeof(fixedchar)*data_type.nvariables);
		delete [] char_buffer;
		delete [] observation_names;
		delete [] variable_names;
//		std::cout << "!!! destr + free !!!\n";
	}
	connected = 0;
	data_file.close();
//	std::cout << "!!! destr !!!\n";
}

template <class DT>
filevector<DT>::filevector(string filename_toload, unsigned long int cachesizeMb): DatABELBaseCPP<DT>::DatABELBaseCPP(filename_toload,cachesizeMb)
{
	connected = 0;
	char_buffer = 0;
	initialize(filename_toload,cachesizeMb);
}

template <class DT>
filevector<DT>::~filevector()
{
    free_resources();
}


template <class DT>
void filevector<DT>::initialize(string filename_toload, unsigned long int cachesizeMb)
{
	if (sizeof(unsigned long int) != 8) warning("you appear to work on 32-bit system... large files not supported\n");

//	if (char_buffer != NULL) error("B: trying to ini already ini-ed object!\n\n");
	if (connected) error("trying to ini already ini-ed object!\n\n");
	filename = filename_toload;
	struct stat filestatus;
	stat( filename_toload.c_str() , &filestatus);

	if (filestatus.st_size < sizeof(data_type))
		error("file %s is too short to contain an FVF-object\n",filename_toload.c_str());

	data_file.open(filename_toload.c_str(), std::ios::out | std::ios::in | std::ios::binary);
	if (data_file.fail())
		error("opening file %s for write & read failed\n",filename_toload.c_str());

	data_file.read((char*)&data_type,sizeof(data_type));
	if (data_file.fail())
        error("failed to read datainfo from file '%s'\n",filename_toload.c_str());

// some integrity checks
	if (sizeof(DT) != data_type.bytes_per_record)
		error("system data type size (%d) and file data type size (%d) do not match\n",
			sizeof(DT),data_type.bytes_per_record);
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

	header_size = sizeof(data_type) + sizeof(fixedchar)*(data_type.nvariables+data_type.nobservations);

	// temp fix because nelements is not yet long ... !!!
//	unsigned long int estimated_size = data_type.bytes_per_record*data_type.nelements + header_size;
	unsigned long int estimated_size =
			(unsigned long int) data_type.bytes_per_record *
			(unsigned long int) data_type.nvariables *
			(unsigned long int) data_type.nobservations + (unsigned long int) header_size;
	if (estimated_size != filestatus.st_size)
		error("actual file size (%lu) differ from the expected (%lu) [%lu,%lu]",
						filestatus.st_size,estimated_size,data_type.nvariables,data_type.nobservations);

// read in variable and observation names

	variable_names = new (std::nothrow) fixedchar [data_type.nvariables];
	if (!variable_names) error("can not get RAM for variable names");
	observation_names = new (std::nothrow) fixedchar [data_type.nobservations];
	if (!observation_names) error("can not get RAM for observation names");
	data_file.seekg(sizeof(data_type), std::ios::beg);
	for (unsigned long int i=0;i<data_type.nobservations;i++)
		data_file.read((char*)(observation_names+i),sizeof(fixedchar));
	for (unsigned long int i=0;i<data_type.nvariables;i++)
		data_file.read((char*)(variable_names+i),sizeof(fixedchar));

    set_cachesizeMb(cachesizeMb);
    update_cache(0);
	connected = 1;
}

template <class DT>
void filevector<DT>::set_cachesizeMb( unsigned long int cachesizeMb )
{
// figure out cache size
	cache_size_Mb = cachesizeMb;
	cache_size_nvars = (unsigned long int) 1024*1024*cache_size_Mb/(data_type.nobservations*data_type.bytes_per_record);
	if (cache_size_nvars<1) {
		message("attempting to set cache size to 1 var (%f Mb)\n",
		         (float) data_type.nobservations*data_type.bytes_per_record/(1024.*1024.));
		cache_size_nvars = 1;
	} else if (cache_size_nvars>data_type.nvariables) {
		message("attempting to cache all the data (%u variables, %f Mb)\n",
			 data_type.nvariables,
		         (float) data_type.nvariables*data_type.nobservations*data_type.bytes_per_record/(1024.*1024.));
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
	char_buffer = new (std::nothrow) char [cache_size_bytes];
	if (!char_buffer)
		error("failed to get memory for cache\n");
	max_buffer_size_bytes = INT_MAX;

	//don't read cache after resizing,
	//it will be updated on next read operation from desired position
	in_cache_from = 0;
	in_cache_to = 0;
}

template <class DT>
void filevector<DT>::update_cache(unsigned long int from_var)
{
	unsigned long int current_cache_size_bytes = cache_size_bytes;
	in_cache_from = from_var;
	in_cache_to = from_var + cache_size_nvars - 1;
	if (in_cache_to >= data_type.nvariables) {
		in_cache_to = data_type.nvariables-1;
		current_cache_size_bytes = (in_cache_to-in_cache_from+1)*
					   data_type.bytes_per_record*data_type.nobservations*sizeof(char);
	}
//	std::cout << "updating cache: " << in_cache_from << " - " << in_cache_to << "\n";
	unsigned long int internal_from = header_size +
			in_cache_from*data_type.nobservations*data_type.bytes_per_record*sizeof(char);
//	std::cout << "position = " << internal_from << "\n";
	data_file.seekg(internal_from, std::ios::beg);
	if (current_cache_size_bytes <= max_buffer_size_bytes) {
		data_file.read((char*)char_buffer,current_cache_size_bytes);
		if (!data_file) error("failed to read cache from file '%s'\n",filename.c_str());
	} else {
// cache size is bigger than what we can read in one go ... read in blocks
		unsigned long int nbytes_togo = current_cache_size_bytes;
		unsigned long int nbytes_finished = 0;
		while (nbytes_togo>0)
		if (nbytes_togo > max_buffer_size_bytes) {
			data_file.read((char*)(char_buffer+nbytes_finished),max_buffer_size_bytes);
			if (!data_file) error("failed to read cache from file '%s'\n",filename.c_str());
			nbytes_finished += max_buffer_size_bytes;
			nbytes_togo -= max_buffer_size_bytes;
		} else {
			data_file.read((char*)(char_buffer+nbytes_finished),nbytes_togo);
			if (!data_file) error("failed to read cache from file '%s'\n",filename.c_str());
			nbytes_finished += nbytes_togo;
			nbytes_togo -= nbytes_togo;
		}
	}
	cached_data = (DT*) char_buffer;
//TMP
//	for (int i=0;i<cache_size_nvars;i++) {
//	for (int j=0;j<data_type.nobservations;j++)
//	std::cout << " " << cached_data[i*data_type.nobservations+j];
//	std::cout<<"\n";
//	}
}

template <class DT>
void filevector<DT>::write_variable_name(unsigned long int nvar, fixedchar name)
{
	if (nvar>=data_type.nvariables) error("trying to set name of obs out of range (%lu)\n\n",nvar);
	variable_names[nvar] = name;
}

template <class DT>
void filevector<DT>::write_observation_name(unsigned long int nobs, fixedchar name)
{
	if (nobs>=data_type.nobservations) error("trying to set name of vars out of range (%lu)\n\n",nobs);
	observation_names[nobs] = name;
}


template <class DT>
fixedchar filevector<DT>::read_variable_name(unsigned long int nvar)
{
	if (nvar>=data_type.nvariables) error("trying to get name of var out of range");
	return(variable_names[nvar]);
}

template <class DT>
fixedchar filevector<DT>::read_observation_name(unsigned long int nobs)
{
	if (nobs>=data_type.nobservations) error("trying to get name of obs out of range");
	return(observation_names[nobs]);
}

// can read single variable
template <class DT>
void filevector<DT>::read_variable(unsigned long int nvar, DT * outvec)
{
	if (nvar>=data_type.nvariables) error("nvar out of range (%lu >= %lu)",nvar,data_type.nvariables);
	if (in_cache_to > 0 && nvar >= in_cache_from && nvar <= in_cache_to) {
		unsigned long int offset = (nvar - in_cache_from)*data_type.nobservations;
		for (unsigned long int i = 0;i<data_type.nobservations;i++) {
			outvec[i]=cached_data[offset+i];
//TMP
//			std::cout << " " << cached_data[offset+i];
		}
	} else {
		update_cache(nvar);
// memcpy from <cstring> may be used here, but not clear if performance will become better
		for (unsigned long int i = 0;i<data_type.nobservations;i++) outvec[i]=cached_data[i];
	}
}



/*template<class DT1,class DT2>
void read_variable_convert_to(filevector<DT1> fv, unsigned long int nvar, DT2 * outvec)
{
DT1 * tmp = new (std::nothrow) DT1[fv.get_nvariables()];
    read_variable(nvar, tmp);
    
} */


template <class DT>
void filevector<DT>::read_observation(unsigned long int nobs, DT * outvec)
{
	DT * tmpdata = new (std::nothrow) DT [get_nobservations()];
	if(!tmpdata)
		error("read_observation: cannot allocate tmpdata");

	for( int i = 0; i< get_nvariables(); i++)
	{
		read_variable(i, tmpdata);
		outvec[i] = tmpdata[nobs];
	}
	delete[] tmpdata;
}

template <class DT>
void filevector<DT>::write_observation(unsigned long int nobs, DT * invec)
{
	for( int i = 0; i< get_nvariables(); i++)
	{
		write_element( i, nobs, invec[i] );
	}
}

// can write single variable
template <class DT>
void filevector<DT>::write_variable(unsigned long int nvar, DT * datavec)
{
	unsigned long int pos = nrnc_to_nelem(nvar, 0);
	data_file.seekp(header_size+pos*sizeof(DT), std::ios::beg);
	data_file.write((char*)datavec,sizeof(DT)*data_type.nobservations);
	if (!data_file) error ("failed to write to data file\n");

	//update data in cache
//	cout << "var:"<< nvar << ",cache from :"<< in_cache_from << ", to: "<< in_cache_to  << endl;

	if (nvar >= in_cache_from && nvar <= in_cache_to)
	{
//	    cout<< "updating data in cache" << endl;
	    unsigned long int offset = (nvar - in_cache_from)*data_type.nobservations;
		for (unsigned long int i = 0;i<data_type.nobservations;i++)
		{
		    cached_data[offset+i]= datavec[i];
        }
	}

//TMP
//	for (unsigned int i=0;i<data_type.nobservations;i++) std::cout << " " << datavec[i];
}

//template <class DT>
//void filevector<DT>::add_variable(DT * invec, fixedchar varname)
//{
//todo
//}

template <class DT>
unsigned long int filevector<DT>::nrnc_to_nelem(unsigned long int nvar, unsigned long int nobs)
{
	if (nvar >= data_type.nvariables || nobs >= data_type.nobservations)
		error("nvar >= real or nobs >= real (%u >= %u || %u >= %u)\n",
			nvar,data_type.nvariables,nobs,data_type.nobservations);
	return( nvar * data_type.nobservations + nobs );
}

// should only be used for reading single random elements!
template <class DT>
DT filevector<DT>::read_element(unsigned long int nvar, unsigned long int nobs)
{
    //todo use cache
	DT out;
	unsigned long int pos = nrnc_to_nelem(nvar, nobs);
	data_file.seekg(header_size+pos*sizeof(DT), std::ios::beg);
	data_file.read((char*)&out,sizeof(DT));
	if (!data_file) error("failed to read an element from file '%s'\n",filename.c_str());
	return(out);
}

template <class DT>
void filevector<DT>::write_element(unsigned long int nvar, unsigned long int nobs, DT data)
{
	unsigned long int pos = nrnc_to_nelem(nvar, nobs);
	data_file.seekp(header_size+pos*sizeof(DT), std::ios::beg);
	data_file.write((char*)&data,sizeof(DT));

	if (nvar >= in_cache_from && nvar <= in_cache_to)
	{
//        cout<< "write_element:updating data in cache" << endl;
		unsigned long int offset = (nvar - in_cache_from)*data_type.nobservations;
		cached_data[offset+nobs]= data;
	}
}

template <class DT>
unsigned int filevector<DT>::get_nvariables()
{
   if(!connected)
   {
       error("cannot return nvariables, not connected\n");
   }
   return data_type.nvariables;
}


template <class DT>
unsigned int filevector<DT>::get_nobservations()
{
   if(!connected)
   {
       error("cannot return nobservations, not connected\n");
   }
   return data_type.nobservations;
}

template <class DT>
void filevector<DT>::save( string new_file_name )
{
    initialize_empty_file( (char *)new_file_name.c_str(), get_nvariables(), get_nobservations(), data_type.type);
    filevector<DT> outdata( new_file_name, 64 );//todo which size for cache to use?

    // copy observation names from the first object
  	for (unsigned long int i=0;i<get_nobservations();i++)
  	    outdata.write_observation_name( i, read_observation_name( i ) );

    DT * tmpvariable = new (std::nothrow) DT[get_nobservations()];
    if (!tmpvariable) error("can not allocate memory for tmpvariable\n\n");

    for (unsigned long int i=0 ; i<get_nvariables();i++)
    {
        //write var names
        outdata.write_variable_name( i, read_variable_name(i));
        //write variables
        read_variable(i,tmpvariable);
        outdata.write_variable(i,tmpvariable);
    }
    delete [] tmpvariable;
}


template <class DT>
void filevector<DT>::save_vars( string new_file_name, unsigned long int nvars, unsigned long int * varindexes)
{
    initialize_empty_file( (char *)new_file_name.c_str(), nvars, get_nobservations(), data_type.type);
    filevector<DT> outdata( new_file_name, 64 );//todo which size for cache to use?

    // copy observation names from the first object
  	for (unsigned long int i=0;i<get_nobservations();i++)
  	    outdata.write_observation_name( i, read_observation_name( i ) );

    DT * tmpvariable = new (std::nothrow) DT[get_nobservations()];
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

template <class DT>
void filevector<DT>::save_obs( string new_file_name, unsigned long int nobss, unsigned long int * obsindexes)
{
    initialize_empty_file( (char *)new_file_name.c_str(), get_nvariables(), nobss, data_type.type);
    filevector<DT> outdata( new_file_name, 64 );//todo which size for cache to use?

    // copy observation names from the first object
  	for( unsigned long int i=0 ; i < nobss ; i++ )
  	    outdata.write_observation_name( i, read_observation_name( obsindexes[i] ) );

    DT * in_variable = new (std::nothrow) DT[get_nobservations()];
    if (!in_variable) error("can not allocate memory for tmpvariable\n\n");

    DT * out_variable = new (std::nothrow) DT[nobss];
    if (!out_variable) error("can not allocate memory for tmpvariable\n\n");
    for ( unsigned long int i=0 ; i<get_nvariables() ; i++ )
    {
        //write var names
        outdata.write_variable_name( i, read_variable_name(i));
        //write variables
        read_variable(i, in_variable);
        //copy data to reduced struct for writing
        for ( unsigned long int j=0 ; j<nobss ; j++ )
        {
            out_variable[j] = in_variable[obsindexes[j]];
        }
        outdata.write_variable(i,out_variable);
    }

    delete [] in_variable;
    delete [] out_variable;
}


#endif

