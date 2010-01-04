#ifndef __DatABELBaseCPP__
#define __DatABELBaseCPP__

#include "frutil.h"

template <class DT> class DatABELBaseCPP
{
    public:
	// HIGH
	// unsigned long int nvariables, nobservations;

    // constructor
    // current:

    DatABELBaseCPP(char * filename_toload, unsigned long int cachesizeMb){};
    //	required:
    DatABELBaseCPP(char * filename_toload, unsigned long int desired_cache_size_Mb, int oFlag, int dbFormat){};

    virtual unsigned int get_nvariables() = 0;
    virtual unsigned int get_nobservations() = 0;

 // can read single variable
	virtual void read_variable(unsigned long int nvar, DT * outvec) = 0;
// should only be used for reading single random elements!
	virtual DT read_element(unsigned long int nvar, unsigned long int nobs) = 0;
// write single variable
// CURRENTLY CACHE IS NOT UPDATED!
	virtual void write_variable(unsigned long int nvar, DT * datavec) = 0;
// write single element
// CURRENTLY CACHE IS NOT UPDATED!
	virtual void write_element(unsigned long int nvar, unsigned long int nobs, DT data) = 0;
// working with names /do not like FIXEDCHAR!/
	virtual fixedchar read_observation_name(unsigned long int nobs) = 0;
	virtual fixedchar read_variable_name(unsigned long int nvar) = 0;

	// These are new functions
// NEW -- TAKE CARE BOTH FILE AND CACHE ARE UPDATED

   // virtual void write_variable_name(unsigned long int nvar,fixedchar newname) = 0;  // loooong future -- control that name is unique
   // virtual void write_observation_name(unsigned long int nobs,fixedchar newname)= 0;  // loooong future -- control that name is unique!
// HIGH -- here I see the possibility to make these functions faster then "random" access functions
   // virtual void add_variable(DT * invec, fixedchar varname) = 0; // adds variable at the end = write_variable with nvar=NVARS?
                                                      // loooong future -- control that name is unique!
// MEDIUM -- see notes on redundant data storage
//	virtual void read_observation(unsigned long int nobs, DT * outvec) = 0;
//    virtual void write_observation(unsigned long int nobs, DT * invec) = 0;

};

#endif


