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
    string inputFile = TestUtil::get_base_dir() + string("/../tests/data/ERF.merlin.22.collected.ped.out.mldose.fvf");
	filevector<float> data( inputFile, 64 );

	std::cout << "Reading file:" << inputFile << endl;

	unsigned long numVariables = data.get_nvariables();
	unsigned long numObservations = data.get_nobservations();

	std::cout << "Size is " << numVariables << " x " << numObservations << endl;

	float* tmpdat = new( nothrow ) float[numObservations];
    string sumFileName = inputFile + string("_varsum");
    ifstream sums(sumFileName.c_str());
    cout << "Reading file: " << sumFileName << endl;

    CPPUNIT_ASSERT(sums.good());

    unsigned long i,j;

	for ( i = 0 ; i < numVariables ; i++ )
	{
	    if (i%2000 == 1){
	        std::cout << i << endl;
	    }
	    data.read_variable(i, tmpdat);
	    float calcSumm, realSumm;
	    sums >> realSumm;
	    
        calcSumm = summData(tmpdat,numObservations);
	    CPPUNIT_ASSERT(TestUtil::relativeDifference(calcSumm,realSumm)<1E-4);
	}

	delete[] tmpdat;

	cout << "Finished" << endl;
}


void CorrectnessTest::testRandomReadObservations(){
    string inputFile = TestUtil::get_base_dir() + string("/../tests/data/ERF.merlin.22.collected.ped.out.mldose.fvf");
    string sumFileName = inputFile + string("_obssum");
	filevector<float> data( inputFile, 64 );

	std::cout << "Reading file:" << inputFile << endl;

	unsigned long numVariables = data.get_nvariables();
	unsigned long numObservations = data.get_nobservations();

	float *tmpdat = new (nothrow) float[numVariables];

	std::cout << "Size is " << numVariables << " x " << numObservations << endl;

	int numObservationsToTest = 10;
	int observationIdx[numObservationsToTest];

	unsigned long i,j;

    TestUtil::initRandomGenerator();
	for (int i=0; i<numObservationsToTest; i++) {
	    observationIdx[i] = (rand()*numObservations)/RAND_MAX;
	}

    ifstream sums(sumFileName.c_str());
    cout << "Reading sum file: " << sumFileName << endl;

    CPPUNIT_ASSERT(sums.good());

    float *sumData = new float[numVariables];
    for(i=0; i<numObservations; i++){
	    sums >> sumData[i];
	}

	for (i = 0 ; i < numObservationsToTest ; i++ )
	{
	    std::cout << i << "(" << observationIdx[i] << ")" << endl;
	    data.read_observation(observationIdx[i], tmpdat);
	    float calcSumm, realSumm;

	    calcSumm = summData(tmpdat, numVariables);
	    CPPUNIT_ASSERT(TestUtil::relativeDifference(calcSumm,sumData[observationIdx[i]]) < 1E-4);
	}

	delete[] tmpdat;
	delete[] sumData;

	cout << "Finished" << endl;
}

void CorrectnessTest::testSubMatrix() {
    string inputFile = TestUtil::get_base_dir() + string("/../tests/data/ERF.merlin.22.collected.ped.out.mldose.fvf");
    string matrixFileName = inputFile + string("_submatrix");
    string obsFileName = inputFile + string("_obsnames");
    string varFileName = inputFile + string("_varnames");

	filevector<float> data( inputFile, 64 );

	ifstream matrixData(matrixFileName.c_str());
	ifstream obsNamesData(obsFileName.c_str());
	ifstream varNamesData(varFileName.c_str());

    CPPUNIT_ASSERT(matrixData.good());
    CPPUNIT_ASSERT(obsNamesData.good());
    CPPUNIT_ASSERT(varNamesData.good());

	cout << "Reading file:" << inputFile << endl;

	unsigned long numVariables = data.get_nvariables();
	unsigned long numObservations = data.get_nobservations();

	unsigned long i, j;

    cout << "Reading observations' names from " << obsFileName << endl;
	map<string, unsigned long> obsToIdx;
	for (i=0; i<numObservations; i++) {
	    string obsName;
	    obsNamesData >> obsName;
	    obsToIdx[obsName] = i;
	}

    cout << "Reading variables' names from " << varFileName << endl;
	map<string, unsigned long> varToIdx;
	for (i=0; i<numVariables; i++) {
	    string varName;
	    varNamesData >> varName;
	    varToIdx[varName] = i;
	}

    // indexes in order, specified in _matrix file.
    vector<unsigned long> obsIdxes;

	cout << "Verifying matrix" << endl;
	for (i=0; i<varToIdx.size(); i++) {
	    string obsName;
	    matrixData >> obsName;
	    obsIdxes.push_back(obsToIdx[obsName]);
	}

    vector<unsigned long> varIdxes;
	for (j=0; j<obsToIdx.size(); j++) {
	    string varName;
	    matrixData >> varName;
	    unsigned long varIdx = varToIdx[varName];

	    for (i = 0; i < numObservations; i++) {
	        float elem;
	        matrixData >> elem;

	        float realElem = data.read_element(varIdx,obsIdxes[i]);

    	    CPPUNIT_ASSERT(TestUtil::relativeDifference(elem,realElem) < 1E-4);
	    }
	}

}


int main(int numArgs, char **args){
    TestUtil::detect_base_dir(string(args[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( CorrectnessTest::suite() );
    runner.run();
    return 0;
}
