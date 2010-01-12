#ifndef __DatABELBaseCPP__
#define __DatABELBaseCPP__

#include <string>

#include "frutil.h"

using namespace std;

template <class DT> class DatABELBaseCPP
{
    public:
    // constructor
    // current:

    DatABELBaseCPP(string filename_toload, unsigned long int cachesizeMb){};
    //	required:
    DatABELBaseCPP(string filename_toload, unsigned long int desired_cache_size_Mb, int oFlag, int dbFormat){};

    // added free_resources

    virtual void free_resources() = 0;

    virtual unsigned int get_nvariables() = 0;
    virtual unsigned int get_nobservations() = 0;

 // can read single variable
	virtual void read_variable(unsigned long int nvar, DT * outvec) = 0;

	template <class DT2>
	void read_variable_convert_to(unsigned long int nvar, DT2 * outvec)
	{
		    DT * tmp = new (std::nothrow) DT[get_nobservations()];
    		if(!tmp)
    			error("read_variable_convert_to allocation error");

    		read_variable(nvar, tmp);

    		for(int i = 0; i< get_nobservations();i++)
    		{
    			outvec[i] = (DT2)tmp[i];
    		}

    		delete[] tmp;
	}

// should only be used for reading single random elements!
	virtual DT read_element(unsigned long int nvar, unsigned long int nobs) = 0;
	// write single variable
	virtual void write_variable(unsigned long int nvar, DT * datavec) = 0;

    // HIGH -- here I see the possibility to make these functions faster then "random" access functions
    // adds variable at the end = write_variable with nvar=NVARS?
	    // todo loooong future -- control that name is unique!
    virtual void add_variable(DT * invec, string varname) = 0; 


// write single element
// CURRENTLY CACHE IS NOT UPDATED!
	virtual void write_element(unsigned long int nvar, unsigned long int nobs, DT data) = 0;
// working with names /do not like FIXEDCHAR!/
	virtual fixedchar read_observation_name(unsigned long int nobs) = 0;
	virtual fixedchar read_variable_name(unsigned long int nvar) = 0;

	// These are new functions
// NEW -- TAKE CARE BOTH FILE AND CACHE ARE UPDATED

   virtual void write_variable_name(unsigned long int nvar, fixedchar newname) = 0;  // todo loooong future -- control that name is unique
   virtual void write_observation_name(unsigned long int nobs, fixedchar newname)= 0;  //todo loooong future -- control that name is unique!


	virtual void read_observation(unsigned long int nobs, DT * outvec) = 0;
    virtual void write_observation(unsigned long int nobs, DT * invec) = 0;

	virtual void save( string new_file_name ) = 0;
	/*
    * Save specified vars to new file
	*/
	virtual void save_vars( string new_file_name, unsigned long int nvars, unsigned long int * varindexes) = 0;
	/*
    * Save specified observations to new file 
	*/
	virtual void save_obs( string new_file_name, unsigned long int nobss, unsigned long int * obsindexes) = 0;

	// changing cache size on the fly
	virtual void set_cachesizeMb( unsigned long int cachesizeMb ) = 0;

};

#endif


