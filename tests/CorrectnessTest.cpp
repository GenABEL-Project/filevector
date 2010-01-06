#include "CorrectnessTest.h"
#include "TestUtil.h"

#include "frvector.h"

using namespace std;

void CorrectnessTest::runTest() {
    string inputFile = TestUtil::get_base_dir() + string("/../tests/data/ERF.merlin.1-22.collected.ped.out.mldose.fvf");
	filevector<float> data( inputFile, 64 );

	std::cout << "Reading file:" << inputFile << endl;

	unsigned long numVariables = data.get_nvariables();
	unsigned long numObservations = data.get_nobservations();

	float* tmpdat = new float[numVariables];

	for ( unsigned long i = 0 ; i < numVariables ; i++ )
	{
		data.read_variable(i, &tmpdat[i]);
	}

	delete[] tmpdat;

	std::cout << "Finished" << endl;

}

int main(){
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( CorrectnessTest::suite() );
  runner.run();
  return 0;
}
