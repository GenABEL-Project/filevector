#include <string>

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

using namespace std;

class CorrectnessTest : public CppUnit::TestFixture {
    void testReadVariable();
    void testRandomReadObservations();
    void testSubMatrix();

    string getInputFileName();

    CPPUNIT_TEST_SUITE( CorrectnessTest );
//    CPPUNIT_TEST( testReadVariable );
  //  CPPUNIT_TEST( testRandomReadObservations );
    CPPUNIT_TEST( testSubMatrix );
    CPPUNIT_TEST_SUITE_END();

public: 
    CorrectnessTest( ) : CppUnit::TestFixture(  ) {}
   
};
                