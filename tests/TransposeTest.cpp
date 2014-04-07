#include "TestUtil.h"
#include "TransposeTest.h"

using namespace std;

void TransposeTest::testTransposeSingleVar() {
    testDbg << "testTransposeSingleVar" << endl;
    string src_file_name = TestUtil::get_temp_file_name();

	string dest_file_name = get_transposed_filename();
	remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
	remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

	unsigned int nvars = 1;
	unsigned int nobs = 3;
	TestUtil::create_empty_filevector(src_file_name,nvars, nobs, INT);

	AbstractMatrix* fv =  new FileVector ( src_file_name, 1 );
	int * var_data = new int[nobs];
	var_data[0] = 1;
	var_data[1] = 2;
	var_data[2] = 3;
	fv->writeVariableAs(0,var_data);
	delete fv;

	Transposer tr;
	tr.process( src_file_name );

	AbstractMatrix* fv_tr =  new FileVector ( dest_file_name, 1 );
    fv_tr->readObservation(0,var_data);
	CPPUNIT_ASSERT_EQUAL(1, var_data[0]);
	CPPUNIT_ASSERT_EQUAL(2, var_data[1]);
	CPPUNIT_ASSERT_EQUAL(3, var_data[2]);

    delete fv_tr;
};

void TransposeTest::testTransposeTwoVars() {
   testDbg << "testTranspose3x3_matrix()" << endl;
   string src_file_name = TestUtil::get_temp_file_name();
   string dest_file_name = get_transposed_filename();
   remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
   remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

   unsigned int nvars = 2;
   unsigned int nobs = 3;
   initializeEmptyFile(src_file_name, nvars, nobs, INT, true);

   AbstractMatrix* fv =  new FileVector ( src_file_name, 1 );
   int var_data[nobs];
   var_data[0] = 1;
   var_data[1] = 2;
   var_data[2] = 3;
   fv->writeVariableAs(0,var_data);
   var_data[0] = 4;
   var_data[1] = 5;
   var_data[2] = 6;
   fv->writeVariableAs(1,var_data);
   delete fv;
   Transposer tr;
   tr.process( src_file_name );

   AbstractMatrix* fv_tr =  new FileVector ( dest_file_name, 1 );
   CPPUNIT_ASSERT_EQUAL((unsigned long)3, fv_tr->getNumVariables());
   CPPUNIT_ASSERT_EQUAL((unsigned long)2, fv_tr->getNumObservations());

   int var[2];
   fv_tr->readVariableAs(0,var);
   CPPUNIT_ASSERT_EQUAL(1, var[0]);
   CPPUNIT_ASSERT_EQUAL(4, var[1]);

   fv_tr->readVariableAs(1,var);
   CPPUNIT_ASSERT_EQUAL(2, var[0]);
   CPPUNIT_ASSERT_EQUAL(5, var[1]);

   fv_tr->readVariableAs(2,var);
   CPPUNIT_ASSERT_EQUAL(3, var[0]);
   CPPUNIT_ASSERT_EQUAL(6, var[1]);

   delete fv_tr;
};

void TransposeTest::testTranspose3x3_matrix()    {
   testDbg << "testTranspose3x3_matrix()" <<endl;
   string src_file_name = TestUtil::get_temp_file_name();

  string dest_file_name = get_transposed_filename();
  remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
  remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

  unsigned int  nvars = 3;
  unsigned int  nobs =3;
  TestUtil::create_empty_filevector(src_file_name,nvars, nobs, INT);

  AbstractMatrix* fv =  new FileVector ( src_file_name, 1 );
  int var_data[nobs];
  var_data[0] = 1;
  var_data[1] = 2;
  var_data[2] = 3;
  fv->writeVariableAs(0,var_data);
  var_data[0] = 4;
  var_data[1] = 5;
  var_data[2] = 6;
  fv->writeVariableAs(1,var_data);
  var_data[0] = 7;
  var_data[1] = 8;
  var_data[2] = 9;
  fv->writeVariableAs(2,var_data);
  delete fv;

  Transposer tr(2);
  tr.process( src_file_name );

  AbstractMatrix* fv_tr =  new FileVector ( dest_file_name, 1 );
  CPPUNIT_ASSERT_EQUAL((unsigned long)3, fv_tr->getNumVariables());
  CPPUNIT_ASSERT_EQUAL((unsigned long)3, fv_tr->getNumObservations());

   int var[3];
   fv_tr->readVariableAs(0,var);
  CPPUNIT_ASSERT_EQUAL(1, var[0]);
  CPPUNIT_ASSERT_EQUAL(4, var[1]);
  CPPUNIT_ASSERT_EQUAL(7, var[2]);

  fv_tr->readVariableAs(1,var);
  CPPUNIT_ASSERT_EQUAL(2, var[0]);
  CPPUNIT_ASSERT_EQUAL(5, var[1]);
  CPPUNIT_ASSERT_EQUAL(8, var[2]);

  fv_tr->readVariableAs(2,var);
  CPPUNIT_ASSERT_EQUAL(3, var[0]);
  CPPUNIT_ASSERT_EQUAL(6, var[1]);
  CPPUNIT_ASSERT_EQUAL(9, var[2]);

   delete fv_tr;

};

void TransposeTest::testTransposeFlatArray() {
   testDbg << "testTransposeFlatArray()" <<endl;
   int  var_data [3][1];
   int  out_data [3][1];
  var_data[0][0] = 1;
  var_data[0][1] = 2;
  var_data[0][2] = 3;

  Transposer tr;
  tr.transpose_part(var_data,out_data,3,1,sizeof(int));
  CPPUNIT_ASSERT_EQUAL(1, out_data[0][0]);
  CPPUNIT_ASSERT_EQUAL(2, out_data[1][0]);
  CPPUNIT_ASSERT_EQUAL(3, out_data[2][0]);
}

void TransposeTest::testTransposeArrayWith2Vars() {
   testDbg << "testTransposeArrayWith2Vars()" <<endl;
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

  Transposer tr;
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
}

int main( int argc, char **argv)
{
    TestUtil::detect_base_dir(string(argv[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( TransposeTest::suite() );
    runner.run();
    return 0;
}