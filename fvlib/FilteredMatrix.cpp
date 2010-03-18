#include <sys/stat.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <map>
#include <memory.h>

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

void FilteredMatrix::writeVariableName(unsigned long varIdx, FixedChar name) {
    nestedMatrix.writeVariableName(filteredToRealRowIdx[varIdx], name);
}

void FilteredMatrix::writeObservationName(unsigned long obsIdx, FixedChar name) {
    nestedMatrix.writeObservationName(filteredToRealColIdx[obsIdx], name);
}

FixedChar FilteredMatrix::readVariableName(unsigned long varIdx) {
    return nestedMatrix.readVariableName(filteredToRealRowIdx[varIdx]);
}

FixedChar FilteredMatrix::readObservationName(unsigned long obsIdx) {
    return nestedMatrix.readObservationName(filteredToRealColIdx[obsIdx]);
}

void FilteredMatrix::readVariable(unsigned long varIdx, void * outvec) {
    unsigned long i;
    for(i=0;i<getNumObservations();i++){
        readElement(varIdx, i, (char*)outvec + i * getElementSize());
    }
}

void FilteredMatrix::readObservation(unsigned long obsIdx, void * outvec) {
    unsigned long i;
    for(i=0;i<getNumVariables();i++){
        readElement( i, obsIdx, (char*)outvec + i * getElementSize());
    }
}

void FilteredMatrix::writeObservation(unsigned long obsIdx, void * invec) {
    unsigned long i;
    for(i=0;i<getNumObservations();i++){
        writeElement( i, obsIdx, (char*)invec + i * getElementSize());
    }
}

void FilteredMatrix::writeVariable(unsigned long varIdx, void *datavec) {
    unsigned long i;
    deepDbg << "FilteredMatrix.writeVariable(" << varIdx << ")" << endl;
    for(i=0;i<getNumObservations();i++){
        writeElement(varIdx, i, (char*)datavec + i * getElementSize());
    }
}

void FilteredMatrix::readElement(unsigned long varIdx, unsigned long obsIdx, void * out) {
    nestedMatrix.readElement(filteredToRealRowIdx[varIdx], filteredToRealColIdx[obsIdx], out);
}

void FilteredMatrix::writeElement(unsigned long varIdx, unsigned long obsIdx, void* data) {
    deepDbg << "FilteredMatrix.writeElement (" << varIdx << "," << obsIdx << ")" << endl;
    nestedMatrix.writeElement(filteredToRealRowIdx[varIdx], filteredToRealColIdx[obsIdx], data);
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



