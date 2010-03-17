#ifndef __FilteredMatrix__
#define __FilteredMatrix__

#include "AbstractMatrix.h"

class FilteredMatrix : public AbstractMatrix {
    AbstractMatrix &nestedMatrix;
    
    map<unsigned long, unsigned long> filteredToRealColIdx;
    map<unsigned long, unsigned long> filteredToRealRowIdx;

    void filterIdxList(unsigned long *iIndexes, unsigned long numIndexes, vector<unsigned long> &oIndexes, map<unsigned long, unsigned long> &filterMap) {
        oIndexes.reserve(filteredToRealColIdx.size());

        unsigned long i;
        for(i=0; i<numIndexes; i++) {
            oIndexes.push_back(filterMap[iIndexes[i]]);
        }
    }
    
public:
    FilteredMatrix(AbstractMatrix &matrix, vector<unsigned long> rowMask, vector<unsigned long> colMask) : nestedMatrix(matrix) {
        unsigned long realColIdx;
        for (realColIdx=0; realColIdx<colMask.size(); realColIdx++) {
           filteredToRealColIdx[realColIdx] = colMask[realColIdx];
        }

        unsigned long realRowIdx;
        for (realRowIdx=0; realRowIdx<rowMask.size(); realRowIdx++) {
            filteredToRealRowIdx[realRowIdx] = rowMask[realRowIdx];
        }
    }
    unsigned int getNumVariables();
    unsigned int getNumObservations();
    void cacheAllNames(bool);

	void saveAs(string newFilename);
	void saveVariablesAs(string newFilename, unsigned long nvars, unsigned long * varindexes);
   	void saveObservationsAs(string newFilename, unsigned long nobss, unsigned long * obsindexes);
    void saveAs(string newFilename, unsigned long nvars, unsigned long nobss, unsigned long * varindexes, unsigned long * obsindexes);
    void saveAsText(string newFilename, unsigned long nvars, unsigned long nobss, unsigned long * varindexes, unsigned long * obsindexes);
    void readObservation(unsigned long obsIdx, void * outvec);
    void writeObservation(unsigned long obsIdx, void * invec);
    void writeVariableName(unsigned long nvar, fixedchar newname);  // todo loooong future -- control that name is unique
    void writeObservationName(unsigned long obsIdx, fixedchar newname);  //todo loooong future -- control that name is unique!
    unsigned long getCacheSizeInMb();
    void setCacheSizeInMb( unsigned long cachesizeMb );
    fixedchar readObservationName(unsigned long obsIdx);
    fixedchar readVariableName(unsigned long nvar);
    void setUpdateNamesOnWrite(bool bUpdate);
	short unsigned getElementSize();
	short unsigned getElementType();
	void readVariable(unsigned long nvar, void * outvec);
	void readElement(unsigned long nvar, unsigned long obsIdx, void * elem);
	void writeVariable(unsigned long nvar, void * datavec);
	void writeElement(unsigned long nvar, unsigned long obsIdx, void * data);

private:
    void addVariable(void * invec, string varname);
};

#endif