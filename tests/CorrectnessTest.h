#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

class CorrectnessTest : public CppUnit::TestFixture {
    void runTest();
    
    CPPUNIT_TEST_SUITE( CorrectnessTest );
    CPPUNIT_TEST( runTest );
    CPPUNIT_TEST_SUITE_END();

public: 
    CorrectnessTest( ) : CppUnit::TestFixture(  ) {}
   
};
                