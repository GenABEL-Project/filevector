#include "TestUtil.h"
#include "TransposeTest.h"

using namespace std;

    void TransposeTest::testTransposeSingleVar()
    {
       cout << "testTransposeSingleVar" << endl;
       string src_file_name = TestUtil::get_temp_file_name();

	   string dest_file_name = get_transposed_filename();
	   remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
	   remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

	   unsigned int nvars = 1;
	   unsigned int nobs = 3;
	   TestUtil::create_empty_filevector(src_file_name,nvars, nobs, INT);

	   AbstractMatrix* fv =  new filevector ( src_file_name, 1 );
	   int * var_data = new int[nobs];
	   var_data[0] = 1;
	   var_data[1] = 2;
	   var_data[2] = 3;
	   fv->write_variable_as(0,var_data);
	   delete fv;

	   Transposer tr;
	   tr.process( src_file_name );

	   AbstractMatrix* fv_tr =  new filevector ( dest_file_name, 1 );
       fv_tr->read_observation(0,var_data);
	   CPPUNIT_ASSERT_EQUAL(1, var_data[0]);
	   CPPUNIT_ASSERT_EQUAL(2, var_data[1]);
	   CPPUNIT_ASSERT_EQUAL(3, var_data[2]);

       delete fv_tr;
	   cout << "end of Test" << endl << endl;
    };

    void TransposeTest::testTransposeTwoVars()
    {
       cout << "testTransposeTwoVars()" << endl;
       string src_file_name = TestUtil::get_temp_file_name();
	   string dest_file_name = get_transposed_filename();
	   remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
	   remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

	   unsigned int nvars = 2;
	   unsigned int nobs = 3;
	   initialize_empty_file(src_file_name, nvars, nobs, INT, true);

	   AbstractMatrix* fv =  new filevector ( src_file_name, 1 );
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
	   Transposer tr;
	   tr.process( src_file_name );
       cout << "18" << endl;

	   AbstractMatrix* fv_tr =  new filevector ( dest_file_name, 1 );
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



int main( int argc, char **argv)
{
    TestUtil::detect_base_dir(string(argv[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( TransposeTest::suite() );
    runner.run();
    return 0;
}