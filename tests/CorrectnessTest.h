#include <string>

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

using namespace std;

class CorrectnessTest : public CppUnit::TestFixture {
    void testReadVariable();
//    void testReadVariableRandom(); // some question remains
    void testRandomReadObservations();
/*    void testReadObservationName();
    void testReadVariableName();*/

    CPPUNIT_TEST_SUITE( CorrectnessTest );
    CPPUNIT_TEST( testReadVariable );
//  CPPUNIT_TEST( testReadVariableRandom ); // some question remains
    CPPUNIT_TEST( testRandomReadObservations );
/*    CPPUNIT_TEST( testReadElement );
    CPPUNIT_TEST( testReadObservationName );
    CPPUNIT_TEST( testReadVariableName );*/
    CPPUNIT_TEST_SUITE_END();

public: 
    CorrectnessTest( ) : CppUnit::TestFixture(  ) {}
   
};
                