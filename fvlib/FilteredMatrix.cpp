#include <sys/stat.h>
#include <string.h>
#include <iostream>

using namespace std;

#include "FilteredMatrix.h"
#include "frutil.h"

unsigned long int FilteredMatrix::getCacheSizeInMb() {
	return nestedMatrix.getCacheSizeInMb();
}

void FilteredMatrix::setCacheSizeInMb( unsigned long int cachesizeMb ) {
    nestedMatrix.setCacheSizeInMb(cachesizeMb);
}

void FilteredMatrix::setUpdateNamesOnWrite(bool bUpdate) {
    nestedMatrix.setUpdateNamesOnWrite(bUpdate);
}

void FilteredMatrix::writeVariableName(unsigned long int nvar, fixedchar name) {
    nestedMatrix.writeVariableName(nvar, name);
}

void FilteredMatrix::writeObservationName(unsigned long int nobs, fixedchar name) {
    nestedMatrix.writeObservationName(nobs, name);
}

fixedchar FilteredMatrix::readVariableName(unsigned long int nvar) {
    return nestedMatrix.readVariableName(nvar);
}

fixedchar FilteredMatrix::readObservationName(unsigned long int nobs) {
    return nestedMatrix.readObservationName(nobs);
}

void FilteredMatrix::readVariable(unsigned long int nvar, void * outvec) {}

void FilteredMatrix::readObservation(unsigned long int nobs, void * outvec) {
    nestedMatrix.readObservation(nobs, outvec);
}

void FilteredMatrix::writeObservation(unsigned long int nobs, void * invec) {
    return nestedMatrix.writeObservation(nobs, invec);
}

void FilteredMatrix::writeVariable(unsigned long int nvar, void * datavec) {}

void FilteredMatrix::readElement(unsigned long int nvar, unsigned long int nobs, void * out) {}

void FilteredMatrix::writeElement(unsigned long int nvar, unsigned long int nobs, void* data) {}

unsigned int FilteredMatrix::getNumVariables() {
   return nestedMatrix.getNumVariables();
}

unsigned int FilteredMatrix::getNumObservations() {
   return nestedMatrix.getNumObservations();
}

void FilteredMatrix::saveAs( string newFilename ) {
    nestedMatrix.saveAs(newFilename);
}

void FilteredMatrix::saveVariablesAs( string newFilename, unsigned long int nvars, unsigned long int * varindexes) {
    nestedMatrix.saveVariablesAs( newFilename, nvars, varindexes);
}

void FilteredMatrix::saveObservationsAs( string newFilename, unsigned long int nobss, unsigned long int * obsindexes)
{
    nestedMatrix.saveObservationsAs(newFilename, nobss, obsindexes);
}

void FilteredMatrix::saveAs(string newFilename, unsigned long int nvars, unsigned long int nobss,
    unsigned long int * varindexes, unsigned long int * obsindexes) {
    nestedMatrix.saveAs(newFilename, nvars, nobss, varindexes, obsindexes);
}

void FilteredMatrix::saveAsText(string newFilename, unsigned long int nvars, unsigned long int nobss,
    unsigned long int * varindexes, unsigned long int * obsindexes) {
    nestedMatrix.saveAsText(newFilename, nvars, nobss, varindexes, obsindexes);
}

short unsigned FilteredMatrix::getElementSize() {
    return getElementSize();
}

short unsigned FilteredMatrix::getDataType() {
    return getDataType();
}

void FilteredMatrix::addVariable(void * invec, string varname) {}



