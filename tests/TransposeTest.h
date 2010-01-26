#ifndef __TRANSPOSETEST__
#define __TRANSPOSETEST__

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <string>
#include <cstring>

#include<stdio.h>

#include "DatABELBaseCPP.h"
#include "filevector.h"
#include "transpose.h"
#include "TestUtil.h"

using namespace std;


/*
* This test is for correctness of filevector operations, while most of other thests are for performance testing
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

    void testTransposeSingleVar()
    {
       string src_file_name = TestUtil::get_temp_file_name();

	   string dest_file_name = get_transposed_filename();
	   remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
	   remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

	   unsigned int  nvars = 1;
	   unsigned int  nobs =3;
	   TestUtil::create_empty_filevector(src_file_name,nvars, nobs, INT);

	   DatABELBaseCPP* fv =  new filevector ( src_file_name, 1 );
	   int * var_data = new int[nobs];
	   var_data[0] = 1;
	   var_data[1] = 2;
	   var_data[2] = 3;
	   fv->write_variable_as(0,var_data);
	   delete fv;

	   transpose tr;
	   tr.process( src_file_name );

	   DatABELBaseCPP* fv_tr =  new filevector ( dest_file_name, 1 );
       fv_tr->read_observation(0,var_data);
	   CPPUNIT_ASSERT_EQUAL(1, var_data[0]);
	   CPPUNIT_ASSERT_EQUAL(2, var_data[1]);
	   CPPUNIT_ASSERT_EQUAL(3, var_data[2]);


       delete fv_tr;

    };

    void testTransposeTwoVars()
    {
       string src_file_name = TestUtil::get_temp_file_name();

	   string dest_file_name = get_transposed_filename();
	   remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
	   remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

	   unsigned int  nvars = 2;
	   unsigned int  nobs =3;
	   TestUtil::create_empty_filevector(src_file_name,nvars, nobs, INT);

	   DatABELBaseCPP* fv =  new filevector ( src_file_name, 1 );
	   int var_data[nobs];
	   var_data[0] = 1;
	   var_data[1] = 2;
	   var_data[2] = 3;
	   fv->write_variable_as(0,var_data);
	   var_data[0] = 4;
	   var_data[1] = 5;
	   var_data[2] = 6;
	   fv->write_variable_as(1,var_data);
	   delete fv;

	   transpose tr;
	   tr.process( src_file_name );

	   DatABELBaseCPP* fv_tr =  new filevector ( dest_file_name, 1 );
	   CPPUNIT_ASSERT_EQUAL((unsigned int)3, fv_tr->get_nvariables());
	   CPPUNIT_ASSERT_EQUAL((unsigned int)2, fv_tr->get_nobservations());

       int var[2];
       fv_tr->read_variable_as(0,var);
	   CPPUNIT_ASSERT_EQUAL(1, var[0]);
	   CPPUNIT_ASSERT_EQUAL(4, var[1]);

	   fv_tr->read_variable_as(1,var);
	   CPPUNIT_ASSERT_EQUAL(2, var[0]);
	   CPPUNIT_ASSERT_EQUAL(5, var[1]);

	   fv_tr->read_variable_as(2,var);
	   CPPUNIT_ASSERT_EQUAL(3, var[0]);
	   CPPUNIT_ASSERT_EQUAL(6, var[1]);

       delete fv_tr;

    };


    void testTranspose3x3_matrix()
    {
       string src_file_name = TestUtil::get_temp_file_name();

	   string dest_file_name = get_transposed_filename();
	   remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
	   remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

	   unsigned int  nvars = 3;
	   unsigned int  nobs =3;
	   TestUtil::create_empty_filevector(src_file_name,nvars, nobs, INT);

	   DatABELBaseCPP* fv =  new filevector ( src_file_name, 1 );
	   int var_data[nobs];
	   var_data[0] = 1;
	   var_data[1] = 2;
	   var_data[2] = 3;
	   fv->write_variable_as(0,var_data);
	   var_data[0] = 4;
	   var_data[1] = 5;
	   var_data[2] = 6;
	   fv->write_variable_as(1,var_data);
	   var_data[0] = 7;
	   var_data[1] = 8;
	   var_data[2] = 9;
	   fv->write_variable_as(2,var_data);
	   delete fv;

	   transpose tr(2);
	   tr.process( src_file_name );

	   DatABELBaseCPP* fv_tr =  new filevector ( dest_file_name, 1 );
	   CPPUNIT_ASSERT_EQUAL((unsigned int)3, fv_tr->get_nvariables());
	   CPPUNIT_ASSERT_EQUAL((unsigned int)3, fv_tr->get_nobservations());

       int var[3];
       fv_tr->read_variable_as(0,var);
	   CPPUNIT_ASSERT_EQUAL(1, var[0]);
	   CPPUNIT_ASSERT_EQUAL(4, var[1]);
	   CPPUNIT_ASSERT_EQUAL(7, var[2]);

	   fv_tr->read_variable_as(1,var);
	   CPPUNIT_ASSERT_EQUAL(2, var[0]);
	   CPPUNIT_ASSERT_EQUAL(5, var[1]);
	   CPPUNIT_ASSERT_EQUAL(8, var[2]);

	   fv_tr->read_variable_as(2,var);
	   CPPUNIT_ASSERT_EQUAL(3, var[0]);
	   CPPUNIT_ASSERT_EQUAL(6, var[1]);
	   CPPUNIT_ASSERT_EQUAL(9, var[2]);

       delete fv_tr;

    };




    void testTransposeFlatArray()
    {
       int  var_data [3][1];
       int  out_data [3][1];
	   var_data[0][0] = 1;
	   var_data[0][1] = 2;
	   var_data[0][2] = 3;

	   transpose tr;
	   tr.transpose_part(var_data,out_data,3,1,sizeof(int));
	   CPPUNIT_ASSERT_EQUAL(1, out_data[0][0]);
	   CPPUNIT_ASSERT_EQUAL(2, out_data[1][0]);
	   CPPUNIT_ASSERT_EQUAL(3, out_data[2][0]);

	   
    }

    void testTransposeArrayWith2Vars()
    {
       int var_data[6] ;
       int  out_data[6];
       //1 2 3
       //4 5 6
	   var_data[0] = 1;
	   var_data[1] = 2;
	   var_data[2] = 3;

       var_data[3] = 4;
	   var_data[4] = 5;
	   var_data[5] = 6;

	   transpose tr;
	   tr.transpose_part(var_data,out_data,3,2,sizeof(int));
	   //1 4
	   //2 5
	   //3 6
	   CPPUNIT_ASSERT_EQUAL(1, out_data[0]);
	   CPPUNIT_ASSERT_EQUAL(4, out_data[1]);

	   CPPUNIT_ASSERT_EQUAL(2, out_data[2]);
	   CPPUNIT_ASSERT_EQUAL(5, out_data[3]);

	   CPPUNIT_ASSERT_EQUAL(3, out_data[4]);
	   CPPUNIT_ASSERT_EQUAL(6, out_data[5]);

//	   delete var_data;
//	   delete out_data;
    }

};

#endif