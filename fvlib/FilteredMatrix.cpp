#include <sys/stat.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

#include "FilteredMatrix.h"
#include "frutil.h"

unsigned long FilteredMatrix::getCacheSizeInMb() {
	return nestedMatrix.getCacheSizeInMb();
}

void FilteredMatrix::setCacheSizeInMb( unsigned long cachesizeMb ) {
    nestedMatrix.setCacheSizeInMb(cachesizeMb);
}

void FilteredMatrix::setUpdateNamesOnWrite(bool bUpdate) {
    nestedMatrix.setUpdateNamesOnWrite(bUpdate);
}

void FilteredMatrix::writeVariableName(unsigned long nvar, fixedchar name) {
    nestedMatrix.writeVariableName(filteredToRealRowIdx[nvar], name);
}

void FilteredMatrix::writeObservationName(unsigned long obsIdx, fixedchar name) {
    nestedMatrix.writeObservationName(filteredToRealColIdx[obsIdx], name);
}

fixedchar FilteredMatrix::readVariableName(unsigned long nvar) {
    return nestedMatrix.readVariableName(filteredToRealRowIdx[nvar]);
}

fixedchar FilteredMatrix::readObservationName(unsigned long obsIdx) {
    return nestedMatrix.readObservationName(filteredToRealColIdx[obsIdx]);
}

void FilteredMatrix::readVariable(unsigned long nvar, void * outvec) { }

void FilteredMatrix::readObservation(unsigned long obsIdx, void * outvec) {
    nestedMatrix.readObservation(filteredToRealColIdx[obsIdx], outvec);
}

void FilteredMatrix::writeObservation(unsigned long obsIdx, void * invec) {
    nestedMatrix.writeObservation(filteredToRealColIdx[obsIdx], invec);
}

void FilteredMatrix::writeVariable(unsigned long nvar, void * datavec) {}

void FilteredMatrix::readElement(unsigned long nvar, unsigned long obsIdx, void * out) {
    nestedMatrix.readElement(filteredToRealRowIdx[nvar], filteredToRealColIdx[obsIdx], out);
}

void FilteredMatrix::writeElement(unsigned long nvar, unsigned long obsIdx, void* data) {
    deepDbg << "FilteredMatrix.writeElement (" << nvar << "," << obsIdx << ")" << endl;
    nestedMatrix.writeElement(filteredToRealRowIdx[nvar], filteredToRealColIdx[obsIdx], data);
}

unsigned int FilteredMatrix::getNumVariables() {
   return filteredToRealRowIdx.size();
}

unsigned int FilteredMatrix::getNumObservations() {
   return filteredToRealColIdx.size();
}

void FilteredMatrix::saveAs(string newFilename) {
    vector<unsigned long> recodedColIndexes;
    vector<unsigned long> recodedRowIndexes;

    map<unsigned long, unsigned long>::iterator i;

    for(i=filteredToRealColIdx.begin(); i!=filteredToRealColIdx.end(); i++) {
        recodedColIndexes.push_back(i->second);
        recodedRowIndexes.push_back(i->second);
    }

    nestedMatrix.saveAs(newFilename, recodedRowIndexes.size(), recodedColIndexes.size(), &recodedRowIndexes[0], &recodedColIndexes[0]);
}

void FilteredMatrix::saveVariablesAs( string newFilename, unsigned long nvars, unsigned long * varIndexes) {
    vector<unsigned long> recodedIndexes;
    filterIdxList(varIndexes, nvars, recodedIndexes, filteredToRealRowIdx);
    nestedMatrix.saveVariablesAs( newFilename, nvars, &recodedIndexes[0]);
}

void FilteredMatrix::saveObservationsAs( string newFilename, unsigned long nobss, unsigned long * obsIndexes) {
    vector<unsigned long> recodedIndexes;
    filterIdxList(obsIndexes, nobss, recodedIndexes, filteredToRealColIdx);
    nestedMatrix.saveObservationsAs(newFilename, nobss, &recodedIndexes[0]);
}

void FilteredMatrix::saveAs(string newFilename, unsigned long nvars, unsigned long nobss, unsigned long *varIndexes, unsigned long *obsIndexes) {
    vector<unsigned long> recodedColIndexes;
    vector<unsigned long> recodedRowIndexes;
    filterIdxList(varIndexes, nobss, recodedColIndexes, filteredToRealColIdx);
    filterIdxList(varIndexes, nvars, recodedRowIndexes, filteredToRealRowIdx);
    nestedMatrix.saveAs(newFilename, nvars, nobss, &recodedRowIndexes[0], &recodedColIndexes[0]);
}

void FilteredMatrix::saveAsText(string newFilename, unsigned long nvars, unsigned long nobss, unsigned long * varIndexes, unsigned long * obsIndexes) {
    vector<unsigned long> recodedColIndexes;
    vector<unsigned long> recodedRowIndexes;
    filterIdxList(obsIndexes, nobss, recodedColIndexes, filteredToRealColIdx);
    filterIdxList(varIndexes, nvars, recodedRowIndexes, filteredToRealRowIdx);
    nestedMatrix.saveAsText(newFilename, nvars, nobss, &recodedRowIndexes[0], &recodedColIndexes[0]);
}

short unsigned FilteredMatrix::getElementSize() {
    return nestedMatrix.getElementSize();
}

short unsigned FilteredMatrix::getElementType() {
    return nestedMatrix.getElementType();
}

void FilteredMatrix::addVariable(void * invec, string varname) {
    errorLog << "FilteredMatrix doesn't support addVariable." << endl << errorExit;
}

void FilteredMatrix::cacheAllNames(bool doCache) {
    nestedMatrix.cacheAllNames(doCache);
}



