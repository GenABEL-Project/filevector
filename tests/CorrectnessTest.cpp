#include <iostream>
#include <fstream>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "CorrectnessTest.h"
#include "TestUtil.h"

#include "filevector.h"

using namespace std;

float summData(float *data, int length){
    int i;
    float sum = 0;
    for(i=0; i<length; i++) {
        sum += data[i];
    }
    return sum;
}

void CorrectnessTest::testReadVariable() {
    string inputFile = TestUtil::get_base_dir() + string("/../tests/data/ERF.merlin.22.collected.ped.out.mldose");
    AbstractMatrix *data =  new filevector( inputFile, 64 );

    dbg << "Reading file:" << inputFile << nl;

    unsigned long numVariables = data->get_nvariables();
    unsigned long numObservations = data->get_nobservations();

    dbg << "Size is " << numVariables << " x " << numObservations << nl;

    float* tmpdat = new( nothrow ) float[numObservations];
    string sumFileName = inputFile + string(".fvf_varsum");
    ifstream sums(sumFileName.c_str());
    dbg << "Reading file: " << sumFileName << nl;

    CPPUNIT_ASSERT(sums.good());

    unsigned long i;

    for ( i = 0 ; i < numVariables ; i++ )
    {
      if (i%1000 == 0)
        dbg << i << nl;
      data->read_variable_as(i, tmpdat);
      float calcSumm, realSumm;
      sums >> realSumm;
	    
      calcSumm = summData(tmpdat,numObservations);
      CPPUNIT_ASSERT(TestUtil::relativeDifference(calcSumm,realSumm)<1E-4);
    }

    delete[] tmpdat;
    delete data;

    dbg << "Finished" << nl;
}


void CorrectnessTest::testRandomReadObservations(){
    string inputFile = TestUtil::get_base_dir() + string("/../tests/data/ERF.merlin.22.collected.ped.out.mldose");
    string sumFileName = inputFile + string(".fvf_obssum");
    AbstractMatrix* data = new filevector ( inputFile, 64 );

    dbg << "Reading file:" << inputFile << nl;

    unsigned long numVariables = data->get_nvariables();
    unsigned long numObservations = data->get_nobservations();

    float *tmpdat = new (nothrow) float[numVariables];

    dbg << "Size is " << numVariables << " x " << numObservations << nl;

    int numObservationsToTest = 10;
    int observationIdx[numObservationsToTest];

    unsigned long i,j;

    TestUtil::initRandomGenerator();
    for (int i=0; i<numObservationsToTest; i++) {
        observationIdx[i] = (rand()*numObservations)/RAND_MAX;
    }

    ifstream sums(sumFileName.c_str());
    dbg << "Reading sum file: " << sumFileName << nl;

    CPPUNIT_ASSERT(sums.good());

    float *sumData = new float[numVariables];
    for(i=0; i<numObservations; i++) {
        sums >> sumData[i];
    }

    for (i = 0 ; i < numObservationsToTest ; i++ )
	{
	    dbg << i << "(" << observationIdx[i] << ")" << nl;
	    //data.read_observation(observationIdx[i], tmpdat);
	    float calcSumm;

	    calcSumm = summData(tmpdat, numVariables);
	    dbg<<calcSumm<<nl;
	    dbg<<sumData[observationIdx[i]]<<nl;
	    CPPUNIT_ASSERT(TestUtil::relativeDifference(calcSumm,sumData[observationIdx[i]]) < 1E-4);
	}

    delete[] tmpdat;
    delete[] sumData;

    delete data;

    dbg << "Finished" << nl;
}


void CorrectnessTest::testSubMatrix() {
    string extension = string (".fvi");
    string fileName = string("/../tests/data/ERF.merlin.22.collected.ped.out.mldose");
    string inputFile = TestUtil::get_base_dir() + fileName;
    string subMatrixFileName = TestUtil::get_base_dir() + fileName + string(".fvf_submatrix");
    string obsFileName = TestUtil::get_base_dir() + fileName + string(".fvf_obsnames");
    string varFileName = TestUtil::get_base_dir() + fileName + string(".fvf_varnames");

    dbg << "obsFileName = " << obsFileName << nl;

    AbstractMatrix *data = new filevector ( inputFile, 64 );

    ifstream subMatrixData(subMatrixFileName.c_str());
    ifstream obsNamesData(obsFileName.c_str());
    ifstream varNamesData(varFileName.c_str());

    CPPUNIT_ASSERT(subMatrixData.good());
    CPPUNIT_ASSERT(obsNamesData.good());
    CPPUNIT_ASSERT(varNamesData.good());

    dbg << "Reading file:" << inputFile << nl;

    unsigned long numVariables = data->get_nvariables();
    unsigned long numObservations = data->get_nobservations();

    unsigned long i, j;

    dbg << "Reading observations' names from " << obsFileName << nl;
    map<string, unsigned long> obsToIdx;
    
    for (i=0; i<numObservations; i++) {
        string obsName;
        obsNamesData >> obsName;
        obsToIdx[obsName] = i;
    }

    dbg << "Reading variables' names from " << varFileName << nl;
    map<string, unsigned long> varToIdx;
    for (i=0; i<numVariables; i++) {
        string varName;
        varNamesData >> varName;
        varToIdx[varName] = i;
    }

    // indexes in order, specified in _submatrix file.
    vector<string> obsIdxesNames;

    dbg << "Matrix size is " << data->get_nobservations() << " x " << data->get_nvariables() << nl;

    string obsNames;

    getline(subMatrixData, obsNames);

    tokenize(obsNames, obsIdxesNames);
    
    dbg << "Submatrix width is:" << obsIdxesNames.size() << nl;

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
	        float matrixElem;
	        float submatrixElem;
	        submatrixElem = atof(subMatrixElements[i+1].c_str());
	        data->read_element_as(varIdx, obsIdexes[i], matrixElem);
	        float relDiff = TestUtil::relativeDifference(matrixElem, submatrixElem);
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
