#include <iostream>
#include <fstream>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../fvlib/FileVector.h"

#include "CorrectnessTest.h"
#include "TestUtil.h"

using namespace std;

double summData(double *data, int length){
    int i;
    double sum = 0;
    for(i=0; i<length; i++) {
        sum += data[i];
    }
    return sum;
}

void CorrectnessTest::testReadVariable() {
    string inputFile = getInputFileName();
    AbstractMatrix *data =  new FileVector( inputFile, 64 );

    testDbg << "Reading file:" << inputFile << endl;

    unsigned long numVariables = data->getNumVariables();
    unsigned long numObservations = data->getNumObservations();

    testDbg << "Size is " << numVariables << " x " << numObservations << endl;

    double* tmpdat = new( nothrow ) double[numObservations];
    string sumFileName = inputFile + string(".fvf_varsum");
    ifstream sums(sumFileName.c_str());
    testDbg << "Reading file: " << sumFileName << endl;

    CPPUNIT_ASSERT(sums.good());

    unsigned long i;

    for ( i = 0 ; i < numVariables ; i++ )
    {
      if (i%1000 == 0)
        testDbg << i << endl;
      data->readVariableAs(i, tmpdat);
      double calcSumm, realSumm;
      sums >> realSumm;
	    
      calcSumm = summData(tmpdat,numObservations);
      CPPUNIT_ASSERT(TestUtil::relativeDifference(calcSumm,realSumm)<1E-4);
    }

    delete[] tmpdat;
    delete data;

    testDbg << "Finished" << endl;
}


void CorrectnessTest::testRandomReadObservations(){
    testDbg << "testRandomReadObservations" << endl;
    string inputFile = getInputFileName();
    string sumFileName = inputFile + string(".fvf_obssum");
    AbstractMatrix* data = new FileVector ( inputFile, 64 );

    testDbg << "Reading file:" << inputFile << endl;

    unsigned long numVariables = data->getNumVariables();
    unsigned long numObservations = data->getNumObservations();

    double *tmpdat = new (nothrow) double[numVariables];

    testDbg << "Size is " << numVariables << " x " << numObservations << endl;

    unsigned long numObservationsToTest = 10;
    int observationIdx[numObservationsToTest];

    unsigned long i;

    TestUtil::initRandomGenerator();
    for (i=0; i<numObservationsToTest; i++) {
        observationIdx[i] = (rand()*numObservations)/RAND_MAX;
    }

    ifstream sums(sumFileName.c_str());
    testDbg << "Reading sum file: " << sumFileName << endl;

    CPPUNIT_ASSERT(sums.good());

    double *sumData = new double[numVariables];
    for(i=0; i<numObservations; i++) {
        sums >> sumData[i];
    }

    for (i = 0 ; i < numObservationsToTest ; i++ )
	{
	    testDbg << i << "(" << observationIdx[i] << ")" << endl;
	    data->readObservation(observationIdx[i], tmpdat);
	    double calcSumm;

	    calcSumm = summData(tmpdat, numVariables);
	    double relDiff = TestUtil::relativeDifference(calcSumm,sumData[observationIdx[i]]);

	    CPPUNIT_ASSERT(relDiff < 1E-2);
	}

    delete[] tmpdat;
    delete[] sumData;

    delete data;

    testDbg << "Finished" << endl;
}

string CorrectnessTest::getInputFileName(){
    return TestUtil::get_base_dir() + string("/../tests/data/correctnessTestData");
}

void CorrectnessTest::testSubMatrix() {
    string fileName = getInputFileName();
    string subMatrixFileName = fileName + string(".fvf_submatrix");
    string obsFileName = fileName + string(".fvf_obsnames");
    string varFileName = fileName + string(".fvf_varnames");

    testDbg << "obsFileName = " << obsFileName << endl;
    testDbg << "subMatrixFileName  = " << subMatrixFileName << endl;

    AbstractMatrix *data = new FileVector ( fileName, 64 );

    ifstream subMatrixData(subMatrixFileName.c_str());
    ifstream obsNamesData(obsFileName.c_str());
    ifstream varNamesData(varFileName.c_str());

    CPPUNIT_ASSERT(subMatrixData.good());
    CPPUNIT_ASSERT(obsNamesData.good());
    CPPUNIT_ASSERT(varNamesData.good());

    testDbg << "Reading file:" << fileName << endl;

    unsigned long numVariables = data->getNumVariables();
    unsigned long numObservations = data->getNumObservations();

    unsigned long i;

    testDbg << "Reading observations' names from " << obsFileName << endl;
    map<string, unsigned long> obsToIdx;
    
    for (i=0; i<numObservations; i++) {
        string obsName;
        obsNamesData >> obsName;
        obsToIdx[obsName] = i;
    }

    testDbg << "Reading variables' names from " << varFileName << endl;
    map<string, unsigned long> varToIdx;
    for (i=0; i<numVariables; i++) {
        string varName;
        varNamesData >> varName;
        varToIdx[varName] = i;
    }

    // indexes in order, specified in _submatrix file.
    vector<string> obsIdxesNames;

    testDbg << "Matrix size is " << data->getNumObservations() << " x " << data->getNumVariables() << endl;

    string obsNames;

    getline(subMatrixData, obsNames);

    tokenize(obsNames, obsIdxesNames);
    
    testDbg << "Submatrix width is:" << obsIdxesNames.size() << endl;

    vector<unsigned long> obsIdexes;

    for (i=0; i<obsIdxesNames.size(); i++){
        obsIdexes.push_back(obsToIdx[obsIdxesNames[i]]);
    }

    vector<unsigned long> varIdxes;
    string subMatrixString;
	while (getline(subMatrixData, subMatrixString)) {
	    string varName;
	    vector<string> subMatrixElements;
	    tokenize(subMatrixString, subMatrixElements);
	    varName = subMatrixElements[0];
	    unsigned long varIdx = varToIdx[varName];

	    for (i = 0; i < obsIdxesNames.size(); i++) {
	        double matrixElem;
	        double submatrixElem;
	        submatrixElem = atof(subMatrixElements[i+1].c_str());
	        data->readElementAs(varIdx, obsIdexes[i], matrixElem);
	        double relDiff = TestUtil::relativeDifference(matrixElem, submatrixElem);
	        CPPUNIT_ASSERT( relDiff = 0./0. || relDiff < 1E-4 );
	    }
	}
    delete data;
}


int main(int numArgs, char **args){
    TestUtil::detect_base_dir(string(args[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( CorrectnessTest::suite() );
    runner.run();
    return 0;
}
