#ifndef __AbstractMatrix__
#define __AbstractMatrix__

#include <string>

using namespace std;

#include "frutil.h"
#include "CastUtils.h"

class AbstractMatrix
{
public:
    virtual ~AbstractMatrix(){};

    template <class DT>
    void writeVariableAs(unsigned long int nvar, DT * outvec)
    {
        char* tmp = new (nothrow) char [getNumObservations()*getElementSize()];
        if(!tmp)
            error("writeVariableAs allocation error");
        for(int i = 0; i< getNumObservations();i++){
            performCast(&tmp[i*getElementSize()],outvec[i],getElementType());
        }
        writeVariable(nvar, tmp);
        delete[] tmp;
    }

    template <class DT>
    void addVariableAs(DT * outvec, string varname)
    {
        char* tmp = new (nothrow) char [getNumObservations()*getElementSize()];
        if(!tmp)
            error("add_variable_as allocation error");
        for(int i = 0; i< getNumObservations();i++){
            performCast(&tmp[i*getElementSize()],outvec[i],getElementType());
        }
        addVariable (tmp, varname);
        delete[] tmp;
    }

    template<class DT>
    void readVariableAs(unsigned long int nvar, DT * outvec)
    {
       char * tmp = new (nothrow) char[getNumObservations()*getElementSize()];
       readVariable(nvar, tmp);
       for(int i = 0; i< getNumObservations();i++) {
            performCast(outvec[i],&tmp[i*getElementSize()],getElementType());
       }
       delete[] tmp;
    }

    void readElementAs(unsigned long varNumber, unsigned long obsNumber, float& element){
        char *ret= new char [getElementSize()];
        readElement(varNumber, obsNumber, ret);
        performCast(element, ret, getElementType());
        delete [] ret;
    }

    template <class DT>
    void writeElementAs(unsigned long varNumber, unsigned long obsNumber, DT& element){
       char *ret= new char [getElementSize()];
       performCast(ret, element, getElementType());
       writeElement(varNumber, obsNumber, ret);
       delete [] ret;
    }

    virtual unsigned int getNumVariables() = 0;
    virtual unsigned int getNumObservations() = 0;

	virtual void saveAs( string newFilename ) = 0;
	virtual void saveVariablesAs( string newFilename, unsigned long int nvars, unsigned long int * varindexes) = 0;
   	virtual void saveObservationsAs( string newFilename, unsigned long int nobss, unsigned long int * obsindexes) = 0;

    virtual void saveAs(string newFilename, unsigned long int nvars, unsigned long int nobss, unsigned long int * varindexes, unsigned long int * obsindexes) = 0;
    virtual void saveAsText(string newFilename, unsigned long int nvars, unsigned long int nobss, unsigned long int * varindexes, unsigned long int * obsindexes) = 0;

    virtual void readObservation(unsigned long int nobs, void * outvec) = 0;
    virtual void writeObservation(unsigned long int nobs, void * invec) = 0;

    virtual void writeVariableName(unsigned long int nvar, fixedchar newname) = 0;  // todo loooong future -- control that name is unique
    virtual void writeObservationName(unsigned long int nobs, fixedchar newname)= 0;  //todo loooong future -- control that name is unique!

    virtual unsigned long int getCacheSizeInMb() = 0;
    virtual void setCacheSizeInMb( unsigned long int cachesizeMb ) = 0;

    virtual fixedchar readObservationName(unsigned long int nobs) = 0;
    virtual fixedchar readVariableName(unsigned long int nvar) = 0;

    virtual void setUpdateNamesOnWrite(bool bUpdate) = 0;

private:
	virtual void readVariable(unsigned long int nvar, void * outvec) = 0;
	virtual void readElement(unsigned long int nvar, unsigned long int nobs, void * elem) = 0;
	virtual void writeVariable(unsigned long int nvar, void * datavec) = 0;

    // HIGH -- here I see the possibility to make these functions faster then "random" access functions
    // adds variable at the end = writeVariable with nvar=NVARS?
	// todo loooong future -- control that name is unique!
    virtual void addVariable(void * invec, string varname) = 0;
    //    virtual void add_observation(void * invec, string obsname) = 0;
    // write single element
    // CURRENTLY CACHE IS NOT UPDATED!
	virtual void writeElement(unsigned long int nvar, unsigned long int nobs, void * data) = 0;

	virtual short unsigned getElementSize() = 0;
	virtual short unsigned getElementType() = 0;
};

#endif


