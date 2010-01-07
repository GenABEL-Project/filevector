#include <iostream>
#include <fstream>
#include <cmath>

#include "CorrectnessTest.h"
#include "TestUtil.h"

#include "frvector.h"

using namespace std;

void CorrectnessTest::testReadVariable() {
    string inputFile = TestUtil::get_base_dir() + string("/../tests/data/ERF.merlin.22.collected.ped.out.mldose.fvf");
	filevector<float> data( inputFile, 64 );

	std::cout << "Reading file:" << inputFile << endl;

	unsigned long numVariables = data.get_nvariables();
	unsigned long numObservations = data.get_nobservations();

	std::cout << "Size is " << numVariables << " x " << numObservations << endl;

	float* tmpdat = new( nothrow ) float[numVariables];
    string sumFileName = inputFile + string("_varsum");
    ifstream sums;
    sums.open(sumFileName.c_str(), ifstream::in);
    cout << "Reading file: " << sumFileName << endl;

    CPPUNIT_ASSERT(sums.good());

    unsigned long i,j;

	for ( i = 0 ; i < numVariables ; i++ )
	{
	    if (i%500000 == 1){
	        std::cout << i << endl;
	    }
	    data.read_variable(i, tmpdat);
	    float calcSumm, realSumm;
	    sums >> realSumm;
        calcSumm = 0;
	    for (j = 0; j < numObservations; j++) {
	        calcSumm += tmpdat[j];
	    }
	    CPPUNIT_ASSERT(TestUtil::relativeDifference(calcSumm,realSumm)<1E-4);
	}

	delete[] tmpdat;

	cout << "Finished" << endl;
}

int main(int numArgs, char **args){
    TestUtil::detect_base_dir(string(args[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( CorrectnessTest::suite() );
    runner.run();
    return 0;
}
