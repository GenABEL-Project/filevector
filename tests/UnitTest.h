#ifndef __UNITTEST__
#define __UNITTEST__

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <string>
#include <cstring>
#include <stdio.h>

#include "../fvlib/FileVector.h"
#include "TestUtil.h"

using namespace std;

/*
* This test is for correctness of FileVector operations, while most of other thests are for performance testing
*/
class UnitTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( UnitTest );
    CPPUNIT_TEST( testCacheUpdatedOnWrite );
    CPPUNIT_TEST( test_write_variable_name );
    CPPUNIT_TEST( test_writeObservationName );
    CPPUNIT_TEST( test_save );
    CPPUNIT_TEST( test_save_vars );
    CPPUNIT_TEST( test_save_obs );
    CPPUNIT_TEST( test_save_vars_obs );
    CPPUNIT_TEST( test_setCacheSizeMb );
    CPPUNIT_TEST( test_read_write_observation );
    CPPUNIT_TEST( test_readVariable_convert_to );
    CPPUNIT_TEST( test_add_variable );
    CPPUNIT_TEST( test_extract_base_file_name );
    CPPUNIT_TEST( testFilteredMatrix );

    CPPUNIT_TEST_SUITE_END();

public:
    string get_file_name_to_write();

    void testCacheUpdatedOnWrite ();
    void test_write_variable_name ();
    void test_writeObservationName ();
    void test_save ();
    void test_save_vars ();
    void test_save_obs ();
    void test_save_vars_obs ();
    void test_setCacheSizeMb ();
    void test_read_write_observation ();
    void test_readVariable_convert_to ();
    void test_add_variable ();
    void test_extract_base_file_name ();
    void testFilteredMatrix();
};



#endif


