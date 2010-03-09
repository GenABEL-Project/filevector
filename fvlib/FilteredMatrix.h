#ifndef __FilteredMatrix__
#define __FilteredMatrix__

#include "AbstractMatrix.h"

class FilteredMatrix : public AbstractMatrix {
    AbstractMatrix &nestedMatrix;
    FilteredMatrix(AbstractMatrix &matrix) : nestedMatrix(matrix) {}
public:
    unsigned int getNumVariables();
    unsigned int getNumObservations();

	void saveAs( string newFilename );
	void saveVariablesAs( string newFilename, unsigned long int nvars, unsigned long int * varindexes);
   	void saveObservationsAs( string newFilename, unsigned long int nobss, unsigned long int * obsindexes);
    void saveAs(string newFilename, unsigned long int nvars, unsigned long int nobss, unsigned long int * varindexes, unsigned long int * obsindexes);
    void saveAsText(string newFilename, unsigned long int nvars, unsigned long int nobss, unsigned long int * varindexes, unsigned long int * obsindexes);
    void readObservation(unsigned long int nobs, void * outvec);
    void writeObservation(unsigned long int nobs, void * invec);
    void writeVariableName(unsigned long int nvar, fixedchar newname);  // todo loooong future -- control that name is unique
    void writeObservationName(unsigned long int nobs, fixedchar newname);  //todo loooong future -- control that name is unique!
    unsigned long int getCacheSizeInMb();
    void setCacheSizeInMb( unsigned long int cachesizeMb );
    fixedchar readObservationName(unsigned long int nobs);
    fixedchar readVariableName(unsigned long int nvar);
    void setUpdateNamesOnWrite(bool bUpdate);

private:
	void readVariable(unsigned long int nvar, void * outvec);
	void readElement(unsigned long int nvar, unsigned long int nobs, void * elem);
	void writeVariable(unsigned long int nvar, void * datavec);
    void addVariable(void * invec, string varname);
	void writeElement(unsigned long int nvar, unsigned long int nobs, void * data);
	short unsigned getElementSize();
	short unsigned getDataType();
};

#endif