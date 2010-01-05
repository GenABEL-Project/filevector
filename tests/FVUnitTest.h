#ifndef __UNITTEST__
#define __UNITTEST__

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <string>

using namespace std;

/*
* This test is for correctness of filevector operations, while most of other thests are for performance testing
*/
class FVUnitTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( FVUnitTest );
    CPPUNIT_TEST( testCacheUpdatedOnWrite );
    CPPUNIT_TEST_SUITE_END();

public:

    string get_file_name_to_write();

    void testCacheUpdatedOnWrite()
    {
        string file_name = get_file_name_to_write();
        filevector<float> fv( file_name, 64 );
        float * var = new (std::nothrow) float [fv.get_nobservations()];
        fv.read_variable(0,var);
        float val = var[0];
        float newVal = val+1.;
        var[0] = newVal;
        fv.write_variable(0,var);

        float * var2 = new (std::nothrow) float [fv.get_nobservations()];
        fv.read_variable(0,var2);

        cout<< "value from read():" << var2[0] << ",newVal: "<<newVal<< endl;
        CPPUNIT_ASSERT_EQUAL( var2[0] , newVal );

    };

};

#endif


