#ifndef __FilteredMatrix__
#define __FilteredMatrix__

#include "AbstractMatrix.h"

class FilteredMatrix : public AbstractMatrix {
    AbstractMatrix &nestedMatrix;
    
    map<unsigned long, unsigned long> filteredToRealColIdx;
    map<unsigned long, unsigned long> filteredToRealRowIdx;
    
    FilteredMatrix(AbstractMatrix &matrix, vector<bool> rowMask, vector<bool> colMask) : nestedMatrix(matrix) {
        unsigned long realColIdx, filteredColIdx;
        for (realColIdx=0; realColIdx<rowMask.size(); realColIdx++) {
            if (rowMask[realColIdx]) {
                filteredToRealColIdx[filteredColIdx] = realColIdx;
                filteredColIdx++;            
            }
        }
        
        unsigned long realRowIdx, filteredRowIdx;
        for (realRowIdx=0; realRowIdx<rowMask.size(); realRowIdx++) {
            if (rowMask[realRowIdx]) {
                filteredToRealRowIdx[filteredRowIdx] = realRowIdx;
                filteredRowIdx++;            
            }
        }
    }

    void filterIdxList(unsigned long *iIndexes, unsigned long numIndexes, vector<unsigned long> &oIndexes, map<unsigned long, unsigned long> &filterMap) {
        oIndexes.reserve(filteredToRealColIdx.size());

        unsigned long i;
        for(i=0; i<numIndexes; i++) {
            oIndexes.push_back(filterMap[iIndexes[i]]);
        }
    }
    
public:
    unsigned int getNumVariables();
    unsigned int getNumObservations();

	void saveAs(string newFilename);
	void saveVariablesAs(string newFilename, unsigned long nvars, unsigned long * varindexes);
   	void saveObservationsAs(string newFilename, unsigned long nobss, unsigned long * obsindexes);
    void saveAs(string newFilename, unsigned long nvars, unsigned long nobss, unsigned long * varindexes, unsigned long * obsindexes);
    void saveAsText(string newFilename, unsigned long nvars, unsigned long nobss, unsigned long * varindexes, unsigned long * obsindexes);
    void readObservation(unsigned long nobs, void * outvec);
    void writeObservation(unsigned long nobs, void * invec);
    void writeVariableName(unsigned long nvar, fixedchar newname);  // todo loooong future -- control that name is unique
    void writeObservationName(unsigned long nobs, fixedchar newname);  //todo loooong future -- control that name is unique!
    unsigned long getCacheSizeInMb();
    void setCacheSizeInMb( unsigned long cachesizeMb );
    fixedchar readObservationName(unsigned long nobs);
    fixedchar readVariableName(unsigned long nvar);
    void setUpdateNamesOnWrite(bool bUpdate);

private:
	void readVariable(unsigned long nvar, void * outvec);
	void readElement(unsigned long nvar, unsigned long nobs, void * elem);
	void writeVariable(unsigned long nvar, void * datavec);
    void addVariable(void * invec, string varname);
	void writeElement(unsigned long nvar, unsigned long nobs, void * data);
	short unsigned getElementSize();
	short unsigned getDataType();
};

#endif