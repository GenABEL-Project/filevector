#ifndef __FilteredMatrix__
#define __FilteredMatrix__

#include <map>

using namespace std;

#include "AbstractMatrix.h"

typedef map<unsigned long, unsigned long> IndexMap;

class FilteredMatrix : public AbstractMatrix {
    AbstractMatrix *nestedMatrix;
    FilteredMatrix *nestedFilteredMatrix;
    bool nestedMatrixIsFiltered;
    
    IndexMap filteredToRealColIdx;
    IndexMap filteredToRealRowIdx;

    void fillUpIndexMap(vector<unsigned long> &v, IndexMap &m, IndexMap &result );

    void filterIdxList(unsigned long *iIndexes, unsigned long numIndexes, vector<unsigned long> &oIndexes, map<unsigned long, unsigned long> &filterMap) {
        oIndexes.reserve(filteredToRealColIdx.size());

        unsigned long i;
        for(i=0; i<numIndexes; i++) {
            oIndexes.push_back(filterMap[iIndexes[i]]);
        }
    }
    
public:
    map<unsigned long, unsigned long> &getFilteredToRealColMap() {
        return filteredToRealColIdx;
    }
    map<unsigned long, unsigned long> &getFilteredToRealRowMap() {
        return filteredToRealRowIdx;
    }

    FilteredMatrix(AbstractMatrix &matrix, vector<unsigned long> rowMask, vector<unsigned long> colMask)
    : nestedMatrix(&matrix),
     nestedMatrixIsFiltered (false)
    {
        unsigned long realColIdx;
        for (realColIdx=0; realColIdx<colMask.size(); realColIdx++) {
           filteredToRealColIdx[realColIdx] = colMask[realColIdx];
        }

        unsigned long realRowIdx;
        for (realRowIdx=0; realRowIdx<rowMask.size(); realRowIdx++) {
            filteredToRealRowIdx[realRowIdx] = rowMask[realRowIdx];
        }
    }

    FilteredMatrix(FilteredMatrix& fm, vector<unsigned long> rowMask, vector<unsigned long> colMask )
        : nestedFilteredMatrix(&fm),
        nestedMatrixIsFiltered (true) {
        fillUpIndexMap(rowMask,fm.getFilteredToRealRowMap(),filteredToRealRowIdx);
        fillUpIndexMap(colMask,fm.getFilteredToRealColMap(),filteredToRealColIdx);
    }

    AbstractMatrix &getNestedMatrix() {
        return nestedMatrixIsFiltered?nestedFilteredMatrix->getNestedMatrix():*nestedMatrix;
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
    void writeVariableName(unsigned long varIdx, FixedChar newname);  // todo loooong future -- control that name is unique
    void writeObservationName(unsigned long obsIdx, FixedChar newname);  //todo loooong future -- control that name is unique!
    unsigned long getCacheSizeInMb();
    void setCacheSizeInMb( unsigned long cachesizeMb );
    FixedChar readObservationName(unsigned long obsIdx);
    FixedChar readVariableName(unsigned long varIdx);
    void setUpdateNamesOnWrite(bool bUpdate);
	short unsigned getElementSize();
	short unsigned getElementType();
	void readVariable(unsigned long varIdx, void * outvec);
	void readElement(unsigned long varIdx, unsigned long obsIdx, void * elem);
	void writeVariable(unsigned long varIdx, void * datavec);
	void writeElement(unsigned long varIdx, unsigned long obsIdx, void * data);

private:
    void addVariable(void * invec, string varname);
};

#endif