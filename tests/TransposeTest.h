#ifndef __TRANSPOSETEST__
#define __TRANSPOSETEST__

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <string>
#include <cstring>

#include <stdio.h>

#include "../fvlib/AbstractMatrix.h"
#include "../fvlib/FileVector.h"
#include "../fvlib/Transposer.h"
#include "TestUtil.h"

using namespace std;

/*
* This test is for correctness of FileVector operations, while most of other tests are
 for performance testing
*/
class TransposeTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TransposeTest );
    CPPUNIT_TEST( testTransposeSingleVar );
    CPPUNIT_TEST( testTransposeTwoVars );
    CPPUNIT_TEST( testTranspose3x3_matrix );
    CPPUNIT_TEST( testTransposeFlatArray );
    CPPUNIT_TEST( testTransposeArrayWith2Vars );
    CPPUNIT_TEST_SUITE_END();

public:
    string get_transposed_filename()
    {
        return TestUtil::get_dir_name_to_write()+"/tmp_transposed";
    }

    void testTransposeSingleVar();
    void testTranspose3x3_matrix();
    void testTransposeTwoVars();
    void testTransposeFlatArray();
    void testTransposeArrayWith2Vars(); 

};

#endif