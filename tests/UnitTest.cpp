#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "FileModificationTest.h"
#include "TestUtil.h"
#include "UnitTest.h"
#include "../fvlib/FilteredMatrix.h"

using namespace std;

string UnitTest::get_file_name_to_write() {
    string ret = TestUtil::get_base_dir() + string("/../tests/data/2write/modify_me");

    return ret;
}

void UnitTest::testCacheUpdatedOnWrite() {
    testDbg << "testCacheUpdatedOnWrite" << endl;
    string file_name = get_file_name_to_write();

    AbstractMatrix *fv = new FileVector( file_name, 2 );//no need in big cache
    float * var = new float [fv->getNumObservations()];
    fv->readVariableAs(0,var);
    float val = var[0];
    float newVal = val+1.;
    var[0] = newVal;
    fv->writeVariableAs(0,var);

    float * var2 = new float [fv->getNumObservations()];
    fv->readVariableAs(0,var2);

    testDbg<< "value from read():" << var2[0] << ",newVal: "<<newVal<< endl;
    CPPUNIT_ASSERT_EQUAL( var2[0] , newVal );
    delete var;
    delete var2;
};

void UnitTest::test_write_variable_name() {
    string file_name = get_file_name_to_write();
    AbstractMatrix* fv = new FileVector( file_name, 2 );//no need in big cache
    FixedChar _fc_varname_saved;
    strcpy(_fc_varname_saved.name,"testvarname");
    fv->writeVariableName( 0, _fc_varname_saved );

    FixedChar _fc_varname_loaded = fv->readVariableName(0);
    CPPUNIT_ASSERT_EQUAL( string(_fc_varname_saved.name), string(_fc_varname_loaded.name) );
    delete fv;

    AbstractMatrix *fv2 = new FileVector ( file_name, 2 );//reopen
    _fc_varname_loaded = fv2->readVariableName(0);
    CPPUNIT_ASSERT_EQUAL( string(_fc_varname_saved.name), string(_fc_varname_loaded.name) );
}

void UnitTest::test_writeObservationName() {
    testDbg << "test_writeObservationName" << endl;
    string file_name = get_file_name_to_write();
    AbstractMatrix* fv = new FileVector( file_name, 2 );//no need in big cache
    FixedChar _fc_obsname_saved;
    strcpy(_fc_obsname_saved.name,"testvarname");
    fv->writeObservationName( 0, _fc_obsname_saved );

    FixedChar _fc_obsname_loaded = fv->readObservationName(0);
    CPPUNIT_ASSERT_EQUAL( string(_fc_obsname_saved.name), string(_fc_obsname_loaded.name) );
    delete fv;

    AbstractMatrix* fv2 = new FileVector( file_name, 2 );//reopen
    _fc_obsname_loaded = fv2->readObservationName(0);
    CPPUNIT_ASSERT_EQUAL( string(_fc_obsname_saved.name), string(_fc_obsname_loaded.name) );
}

void UnitTest::test_save() {
    testDbg << "test_save" << endl;
    string src_file_name = get_file_name_to_write();

    string dest_file_name = TestUtil::get_dir_name_to_write()+"/save_test";
    remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
    remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

    AbstractMatrix* fv =  new FileVector ( src_file_name, 2 );//no need in big cache
    fv->saveAs( dest_file_name );

    AbstractMatrix* fv_copy = new FileVector ( dest_file_name, 2 );
    CPPUNIT_ASSERT_EQUAL(fv->getNumObservations(),fv_copy->getNumObservations());
    CPPUNIT_ASSERT_EQUAL(fv->getNumVariables(),fv_copy->getNumVariables());

    for(unsigned long i=0;i<fv->getNumVariables();i++)
    {
        CPPUNIT_ASSERT_EQUAL( string(fv->readVariableName( i ).name),string(fv_copy->readVariableName( i ).name));
    }

    for(unsigned long i=0;i<fv->getNumObservations();i++)
    {
        CPPUNIT_ASSERT_EQUAL( string(fv->readObservationName( i ).name),string(fv_copy->readObservationName( i ).name));
    }
    delete fv;
}

void UnitTest::test_save_vars() {
    testDbg << "test_save_vars" << endl; 
    string src_file_name = get_file_name_to_write();
    string dest_file_name = TestUtil::get_dir_name_to_write()+"/save_vars_test";
    remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
    remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

    AbstractMatrix *fv = new FileVector ( src_file_name, 64 );
    unsigned long obs_indexes[2] =  {1,3};
    fv->saveVariablesAs( dest_file_name, 2, obs_indexes );

    AbstractMatrix *fv_copy = new FileVector( dest_file_name, 2 );
    CPPUNIT_ASSERT_EQUAL(fv->getNumObservations(),fv_copy->getNumObservations());
    CPPUNIT_ASSERT_EQUAL( (unsigned long)2 ,fv_copy->getNumVariables() );
}

void UnitTest::test_save_obs() {
    testDbg << "test_save_obs" << endl;
    string src_file_name = get_file_name_to_write();
    string dest_file_name = TestUtil::get_dir_name_to_write()+"/save_obs_test";
    remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
    remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

    AbstractMatrix *fv = new FileVector( src_file_name, 64 );
    unsigned long orig_nvars = fv->getNumVariables();
    float * orig_var = new float [fv->getNumObservations()];
    fv->readVariableAs(1, orig_var);

    unsigned long obs_indexes[2] =  {1,3};
    fv->saveObservationsAs( dest_file_name, 2, obs_indexes );
    delete fv;

    AbstractMatrix* fv_copy = new FileVector( dest_file_name, 2 );
    CPPUNIT_ASSERT_EQUAL((unsigned long )2 ,fv_copy->getNumObservations());
    CPPUNIT_ASSERT_EQUAL( orig_nvars ,fv_copy->getNumVariables() );
    float * saved_var = new float [fv_copy->getNumObservations()];
    fv_copy->readVariableAs(1, saved_var);
    CPPUNIT_ASSERT_EQUAL( orig_var[1] ,saved_var[0] );
    CPPUNIT_ASSERT_EQUAL( orig_var[3] ,saved_var[1] );

    delete fv_copy;
    delete orig_var;
    delete saved_var;
}

/*test saving vars/obs window*/
void UnitTest::test_save_vars_obs() {
    testDbg << "test_save_vars_obs" << endl;
	string src_file_name = get_file_name_to_write();

	string dest_file_name = TestUtil::get_dir_name_to_write()+"/save_test";
	remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
	remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

	AbstractMatrix* fv =  new FileVector ( src_file_name, 2 );//no need in big cache

	unsigned long var_indexes[3] =  {1,7,18};
	unsigned long obs_indexes[2] =  {1,3};
	fv->saveAs( dest_file_name , 3, 2, var_indexes,obs_indexes);

	AbstractMatrix* fv_copy = new FileVector( dest_file_name, 2 );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong number of variables in fv_copy", (unsigned long )3,fv_copy->getNumVariables());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong number of observations in fv_copy", (unsigned long )2 ,fv_copy->getNumObservations());

    float * orig_var = new float [fv->getNumObservations()];
    float * saved_var = new float [fv_copy->getNumObservations()];

    for(int i = 0 ;i<3 ; i++ )
    {
		fv->readVariableAs(var_indexes[i], orig_var);
		fv_copy->readVariableAs(i, saved_var);
    	for(int j = 0 ;j<2 ; j++ )
    	{
			CPPUNIT_ASSERT_EQUAL( orig_var[obs_indexes[j]] ,saved_var[j] );
		}
    }

    delete fv;
    delete fv_copy;
    delete orig_var;
    delete saved_var;
}

void UnitTest::test_setCacheSizeMb() {
    testDbg << "test_setCacheSizeMb" << endl;
    string src_file_name = get_file_name_to_write();
    FileVector *fv = new FileVector( src_file_name, 64 );

    unsigned long vars_capacity =11389 ;
    CPPUNIT_ASSERT_EQUAL( vars_capacity ,fv->cache_size_nvars );

    float * var = new float [fv->getNumObservations()];
    fv->readVariable(0,var);
    CPPUNIT_ASSERT_EQUAL((unsigned long )0 ,fv->in_cache_from );
    CPPUNIT_ASSERT_EQUAL( vars_capacity-1, fv->in_cache_to );


    fv->setCacheSizeInMb(1);
    CPPUNIT_ASSERT_EQUAL((unsigned long )177 ,fv->cache_size_nvars );
    CPPUNIT_ASSERT_EQUAL((unsigned long )0 ,fv->in_cache_from );
    CPPUNIT_ASSERT_EQUAL((unsigned long )0 ,fv->in_cache_to );

    fv->readVariable(fv->getNumVariables() - 1,var);

    CPPUNIT_ASSERT_EQUAL((unsigned long )fv->getNumVariables() - 1,fv->in_cache_from );
    CPPUNIT_ASSERT_EQUAL((unsigned long )fv->getNumVariables() - 1,fv->in_cache_to );

    delete var;
}

void UnitTest::test_read_write_observation() {
    testDbg << "test_read_write_observation" << endl;
    unsigned long numVariables = 5;
	unsigned long nobservations = 3;

    string tmp_file_name = TestUtil::get_temp_file_name();
    TestUtil::create_empty_filevector(tmp_file_name.c_str(), numVariables, nobservations);
	AbstractMatrix* fv = new FileVector( tmp_file_name, 1 );

    float * var1 = new float [fv->getNumVariables()];
    float * var2 = new float [fv->getNumVariables()];
    int i;
    for(i = 0; i<fv->getNumVariables(); i++)
    {
        var1[i] = i;
	}

    fv->writeObservation(2, var1);
    fv->readObservation(2, var2);

    CPPUNIT_ASSERT( TestUtil::compare_arrays(var1, var2, fv->getNumObservations()));
    delete fv;

	//reopen file and check
	AbstractMatrix* fv2 = new FileVector( tmp_file_name, 1 );
	fv2->readObservation(2,var2);
	CPPUNIT_ASSERT( TestUtil::compare_arrays(var1, var2 , numVariables));
    delete fv2;

    delete [] var1;
    delete [] var2;
}


void UnitTest::test_readVariable_convert_to() {
    testDbg << "test_readVariable_convert_to" << endl;
	unsigned long numVariables =10;
	unsigned long nobservations =3;
	string tmp_file_name = TestUtil::get_temp_file_name();
	TestUtil::create_empty_filevector(tmp_file_name,numVariables, nobservations);

	AbstractMatrix* fv = new FileVector( tmp_file_name, 1 );

	float * var = new float [fv->getNumObservations()];
	for(int i = 0; i<fv->getNumObservations(); i++)
    {
        var[i] = i + (float)i/10;
	}
	fv->writeVariableAs(1,var);

	int * int_var = new int[fv->getNumObservations()];
	fv->readVariableAs(1,int_var);

	CPPUNIT_ASSERT_EQUAL(0, int_var[0]);
	CPPUNIT_ASSERT_EQUAL(1, int_var[1]);
	CPPUNIT_ASSERT_EQUAL(2, int_var[2]);

	delete[] var;
	delete[] int_var;
}

void UnitTest::test_add_variable() {
    testDbg << "test_add_variable" << endl;
	string tempFileName = TestUtil::get_temp_file_name();
	TestUtil::create_empty_filevector(tempFileName,10,20);
	AbstractMatrix* fv = new FileVector( tempFileName, 1 );
	float * var = new float [fv->getNumObservations()];
	TestUtil::create_and_fill_variable(fv->getNumObservations(),var);

    string varname = "added";
    string varname2 = "added2";
	fv->addVariableAs(var,varname);
	fv->addVariableAs(var,varname2);

	CPPUNIT_ASSERT_EQUAL((unsigned long)12, fv->getNumVariables());
	FixedChar _fc_varname_loaded = fv->readVariableName(10);
    CPPUNIT_ASSERT_EQUAL( varname, string(_fc_varname_loaded.name) );
    _fc_varname_loaded = fv->readVariableName(11);
    CPPUNIT_ASSERT_EQUAL( varname2, string(_fc_varname_loaded.name) );
    delete fv;

    testDbg << "Part 2" << endl;
    tempFileName = TestUtil::get_temp_file_name();
    AbstractMatrix* fv2 = new FileVector( tempFileName, 1 );

    CPPUNIT_ASSERT_EQUAL((unsigned long)12, fv2->getNumVariables());
	_fc_varname_loaded = fv2->readVariableName(10);
    CPPUNIT_ASSERT_EQUAL( varname, string(_fc_varname_loaded.name) );
    _fc_varname_loaded = fv2->readVariableName(11);
    CPPUNIT_ASSERT_EQUAL( varname2, string(_fc_varname_loaded.name) );
    delete fv2;
}

void UnitTest::test_extract_base_file_name()  {
    testDbg << "test_extract_base_file_name" << endl;
    string fn = "//...///dd/d///filename.fvi";
    string base = extract_base_file_name(fn);
    CPPUNIT_ASSERT_EQUAL( fn , base + ".fvi" );

    fn = "/path/filename.ext1.ext2.ext3.fvi";
    base = extract_base_file_name(fn);
    CPPUNIT_ASSERT_EQUAL( fn , base + ".fvi" );

    fn = "/path/../filename";
    base = extract_base_file_name(fn);
    CPPUNIT_ASSERT_EQUAL( fn , base );
}

void UnitTest::testFilteredMatrix() {
    testDbg << "testFilteredMatrix()" << endl;
	string filename = TestUtil::get_temp_file_name();
	TestUtil::create_empty_filevector(filename, 5, 4);
	FileVector fv(filename,1);

	unsigned long i,j;
	float f = 0;

	for (i=0;i<5;i++){
	    for (j=0;j<4;j++){
        	fv.writeElementAs(i,j,f);
	        f++;
	    }
	}

	vector<unsigned long> rFilter,cFilter;
	rFilter.push_back(0);
	rFilter.push_back(2);
	rFilter.push_back(3);
	cFilter.push_back(1);
	cFilter.push_back(3);

	FilteredMatrix fm(fv);

	fm.setFilteredArea(rFilter,cFilter);

    fm.readElementAs(0,0,f); CPPUNIT_ASSERT_EQUAL(f,(float)1);
    fm.readElementAs(0,1,f); CPPUNIT_ASSERT_EQUAL(f,(float)3);
    fm.readElementAs(1,0,f); CPPUNIT_ASSERT_EQUAL(f,(float)9);
    fm.readElementAs(1,1,f); CPPUNIT_ASSERT_EQUAL(f,(float)11);
    fm.readElementAs(2,0,f); CPPUNIT_ASSERT_EQUAL(f,(float)13);
    fm.readElementAs(2,1,f); CPPUNIT_ASSERT_EQUAL(f,(float)15);

    float var[2] = {12,34};

    fm.writeVariableAs(2,var);

    fv.readElementAs(3,1,f); CPPUNIT_ASSERT_EQUAL(f,(float)12);
    fv.readElementAs(3,3,f); CPPUNIT_ASSERT_EQUAL(f,(float)34);

    float obs[3];

    fm.readObservation(0,obs);
    CPPUNIT_ASSERT_EQUAL((float)1,obs[0]);
    CPPUNIT_ASSERT_EQUAL((float)9,obs[1]);
    CPPUNIT_ASSERT_EQUAL((float)12,obs[2]);

	vector<unsigned long> rFilter2,cFilter2;
	rFilter2.push_back(3);
	rFilter2.push_back(4);
	cFilter2.push_back(0);
	cFilter2.push_back(1);

	testDbg << "Nested FilteredMatrix testing" << endl;

	FilteredMatrix fm2(fm);
	fm2.setFilteredArea(rFilter2, cFilter2);

	fm2.readElementAs(2,0,f); CPPUNIT_ASSERT_EQUAL((float)12,f);
	fm2.readElementAs(2,1,f); CPPUNIT_ASSERT_EQUAL((float)12,f);
	fm2.readElementAs(2,2,f); CPPUNIT_ASSERT_EQUAL((float)34,f);

    testDbg << "End of FilteredMatrix test" << endl;
}

int main( int argc, char **argv)
{
    TestUtil::detect_base_dir(string(argv[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( UnitTest::suite() );

    runner.run();
    return 0;
}


