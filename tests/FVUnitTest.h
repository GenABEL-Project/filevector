#ifndef __UNITTEST__
#define __UNITTEST__

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <string>
#include <cstring>


#include "frvector.h"

using namespace std;

/*
* This test is for correctness of filevector operations, while most of other thests are for performance testing
*/
class FVUnitTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( FVUnitTest );
    CPPUNIT_TEST( testCacheUpdatedOnWrite );
    CPPUNIT_TEST( test_write_variable_name );
    CPPUNIT_TEST( test_write_observation_name );
    CPPUNIT_TEST( test_save );
    CPPUNIT_TEST_SUITE_END();

public:

    string get_file_name_to_write();

    void testCacheUpdatedOnWrite()
    {
        string file_name = get_file_name_to_write();
        filevector<float> fv( file_name, 2 );//no need in big cache
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

    void test_write_variable_name()
    {
        string file_name = get_file_name_to_write();
        filevector<float> fv( file_name, 2 );//no need in big cache
        fixedchar _fc_varname_saved;
        strcpy(_fc_varname_saved.name,"testvarname");
        fv.write_variable_name( 0, _fc_varname_saved );

        fixedchar _fc_varname_loaded = fv.read_variable_name(0);
        CPPUNIT_ASSERT_EQUAL( string(_fc_varname_saved.name), string(_fc_varname_loaded.name) );
        fv.free_resources();

        filevector<float> fv2( file_name, 2 );//reopen
        _fc_varname_loaded = fv2.read_variable_name(0);
        CPPUNIT_ASSERT_EQUAL( string(_fc_varname_saved.name), string(_fc_varname_loaded.name) );
        
    }
    void test_write_observation_name()
    {
        string file_name = get_file_name_to_write();
        filevector<float> fv( file_name, 2 );//no need in big cache
        fixedchar _fc_obsname_saved;
        strcpy(_fc_obsname_saved.name,"testvarname");
        fv.write_observation_name( 0, _fc_obsname_saved );

        fixedchar _fc_obsname_loaded = fv.read_observation_name(0);
        CPPUNIT_ASSERT_EQUAL( string(_fc_obsname_saved.name), string(_fc_obsname_loaded.name) );
        fv.free_resources();

        filevector<float> fv2( file_name, 2 );//reopen
        _fc_obsname_loaded = fv2.read_observation_name(0);
        CPPUNIT_ASSERT_EQUAL( string(_fc_obsname_saved.name), string(_fc_obsname_loaded.name) );
    }

    void test_save()
    {
        string src_file_name = get_file_name_to_write();
        string dest_file_name = src_file_name + "_copy";
        filevector<float> fv( src_file_name, 2 );//no need in big cache
        fv.save( dest_file_name );

        filevector<float> fv_copy( dest_file_name, 2 );
        CPPUNIT_ASSERT_EQUAL(fv.get_nobservations(),fv_copy.get_nobservations());
        CPPUNIT_ASSERT_EQUAL(fv.get_nvariables(),fv_copy.get_nvariables());

        for(unsigned long int i=0;i<fv.get_nvariables();i++)
        {
            CPPUNIT_ASSERT_EQUAL( string(fv.read_variable_name( i ).name),string(fv_copy.read_variable_name( i ).name));
        }

        for(unsigned long int i=0;i<fv.get_nobservations();i++)
        {
            CPPUNIT_ASSERT_EQUAL( string(fv.read_observation_name( i ).name),string(fv_copy.read_observation_name( i ).name));
        }
        
    }
};



#endif


