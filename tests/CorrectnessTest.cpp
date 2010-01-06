#include "DatABELBaseCPP.h"
#include "CorrectnessTest.h"
#include "TestUtil.h"

using namespace std;

void CorrectnessTest::runTest(){
//  cout << "test" << endl;
//    DatAbelBaseCPP fv = filevector();
}

int main(){
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( CorrectnessTest::suite() );
  runner.run();
  return 0;
}
