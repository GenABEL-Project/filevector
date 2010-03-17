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
    void writeVariableAs(unsigned long nvar, DT * outvec)
    {
        char* tmp = new (nothrow) char [getNumObservations()*getElementSize()];
        if(!tmp)
            errorLog << "writeVariableAs allocation error" << errorExit;
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
            errorLog << "add_variable_as allocation error" << errorExit;
        for(int i = 0; i< getNumObservations();i++){
            performCast(&tmp[i*getElementSize()],outvec[i],getElementType());
        }
        addVariable (tmp, varname);
        delete[] tmp;
    }

    template<class DT>
    void readVariableAs(unsigned long nvar, DT * outvec)
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
       deepDbg << "AbstractMatrix.writeElementAs(" << varNumber << "," << obsNumber << "," << element <<")";
       deepDbg << "Alloc getElementSize() = " << getElementSize() << endl;
       char *ret = new char [getElementSize()];
       deepDbg << "Perform cast" << endl;
       performCast(ret, element, getElementType());
       writeElement(varNumber, obsNumber, ret);
       delete [] ret;
    }

    virtual unsigned int getNumVariables() = 0;
    virtual unsigned int getNumObservations() = 0;

	virtual void saveAs( string newFilename ) = 0;
	virtual void saveVariablesAs( string newFilename, unsigned long nvars, unsigned long * varindexes) = 0;
   	virtual void saveObservationsAs( string newFilename, unsigned long nobss, unsigned long * obsindexes) = 0;

    virtual void saveAs(string newFilename, unsigned long nvars, unsigned long nobss, unsigned long * varindexes, unsigned long * obsindexes) = 0;
    virtual void saveAsText(string newFilename, unsigned long nvars, unsigned long nobss, unsigned long * varindexes, unsigned long * obsindexes) = 0;

    virtual void readObservation(unsigned long obsIdx, void * outvec) = 0;
    virtual void writeObservation(unsigned long obsIdx, void * invec) = 0;

    virtual void writeVariableName(unsigned long nvar, fixedchar newname) = 0;  // todo loooong future -- control that name is unique
    virtual void writeObservationName(unsigned long obsIdx, fixedchar newname)= 0;  //todo loooong future -- control that name is unique!

    virtual unsigned long getCacheSizeInMb() = 0;
    virtual void setCacheSizeInMb( unsigned long cachesizeMb ) = 0;

    virtual fixedchar readObservationName(unsigned long obsIdx) = 0;
    virtual fixedchar readVariableName(unsigned long nvar) = 0;
    virtual void cacheAllNames(bool) = 0;

    virtual void setUpdateNamesOnWrite(bool bUpdate) = 0;
	virtual short unsigned getElementSize() = 0;
	virtual short unsigned getElementType() = 0;
	virtual void readVariable(unsigned long nvar, void * outvec) = 0;
	virtual void readElement(unsigned long nvar, unsigned long obsIdx, void * elem) = 0;
	virtual void writeVariable(unsigned long nvar, void * datavec) = 0;
	virtual void writeElement(unsigned long nvar, unsigned long obsIdx, void * data) = 0;
private:

    // HIGH -- here I see the possibility to make these functions faster then "random" access functions
    // adds variable at the end = writeVariable with nvar=NVARS?
	// todo loooong future -- control that name is unique!
    virtual void addVariable(void * invec, string varname) = 0;
    //    virtual void add_observation(void * invec, string obsname) = 0;
    // write single element
    // CURRENTLY CACHE IS NOT UPDATED!
};

#endif


