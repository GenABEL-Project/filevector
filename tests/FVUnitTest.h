#ifndef __UNITTEST__
#define __UNITTEST__

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <string>
#include <cstring>

#include<stdio.h>

#include "filevector.h"

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
    CPPUNIT_TEST( test_save_vars );
    CPPUNIT_TEST( test_save_obs );
    CPPUNIT_TEST( test_save_vars_obs );
    CPPUNIT_TEST( test_set_cachesizeMb );
    CPPUNIT_TEST( test_read_write_observation );
    CPPUNIT_TEST( test_read_variable_convert_to );
    CPPUNIT_TEST( test_add_variable );
    CPPUNIT_TEST( test_extract_base_file_name );

    CPPUNIT_TEST_SUITE_END();

public:

    string get_file_name_to_write();
    string get_dir_name_to_write();
    string get_temp_file_name();

    void testCacheUpdatedOnWrite()
    {
        string file_name = get_file_name_to_write();
        DatABELBaseCPP *fv = new filevector( file_name, 2 );//no need in big cache
        float * var = new float [fv->get_nobservations()];
        fv->read_variable_as(0,var);
        float val = var[0];
        float newVal = val+1.;
        var[0] = newVal;
        fv->write_variable_as(0,var);

        float * var2 = new float [fv->get_nobservations()];
        fv->read_variable_as(0,var2);

        cout<< "value from read():" << var2[0] << ",newVal: "<<newVal<< endl;
        CPPUNIT_ASSERT_EQUAL( var2[0] , newVal );
        delete var;
        delete var2;
    };

    void test_write_variable_name()
    {
        string file_name = get_file_name_to_write();
        DatABELBaseCPP* fv = new filevector( file_name, 2 );//no need in big cache
        fixedchar _fc_varname_saved;
        strcpy(_fc_varname_saved.name,"testvarname");
        fv->write_variable_name( 0, _fc_varname_saved );

        fixedchar _fc_varname_loaded = fv->read_variable_name(0);
        CPPUNIT_ASSERT_EQUAL( string(_fc_varname_saved.name), string(_fc_varname_loaded.name) );
        delete fv;

        DatABELBaseCPP *fv2 = new filevector ( file_name, 2 );//reopen
        _fc_varname_loaded = fv2->read_variable_name(0);
        CPPUNIT_ASSERT_EQUAL( string(_fc_varname_saved.name), string(_fc_varname_loaded.name) );

    }
    void test_write_observation_name()
    {
        string file_name = get_file_name_to_write();
        DatABELBaseCPP* fv = new filevector( file_name, 2 );//no need in big cache
        fixedchar _fc_obsname_saved;
        strcpy(_fc_obsname_saved.name,"testvarname");
        fv->write_observation_name( 0, _fc_obsname_saved );

        fixedchar _fc_obsname_loaded = fv->read_observation_name(0);
        CPPUNIT_ASSERT_EQUAL( string(_fc_obsname_saved.name), string(_fc_obsname_loaded.name) );
        delete fv;

        DatABELBaseCPP* fv2 = new filevector( file_name, 2 );//reopen
        _fc_obsname_loaded = fv2->read_observation_name(0);
        CPPUNIT_ASSERT_EQUAL( string(_fc_obsname_saved.name), string(_fc_obsname_loaded.name) );
    }

    void test_save()
    {
        string src_file_name = get_file_name_to_write();

        string dest_file_name = get_dir_name_to_write()+"/save_test";
        remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
        remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

        DatABELBaseCPP* fv =  new filevector ( src_file_name, 2 );//no need in big cache
        fv->save( dest_file_name );

        DatABELBaseCPP* fv_copy = new filevector ( dest_file_name, 2 );
        CPPUNIT_ASSERT_EQUAL(fv->get_nobservations(),fv_copy->get_nobservations());
        CPPUNIT_ASSERT_EQUAL(fv->get_nvariables(),fv_copy->get_nvariables());

        for(unsigned long int i=0;i<fv->get_nvariables();i++)
        {
            CPPUNIT_ASSERT_EQUAL( string(fv->read_variable_name( i ).name),string(fv_copy->read_variable_name( i ).name));
        }

        for(unsigned long int i=0;i<fv->get_nobservations();i++)
        {
            CPPUNIT_ASSERT_EQUAL( string(fv->read_observation_name( i ).name),string(fv_copy->read_observation_name( i ).name));
        }
        delete fv;
    }

    void test_save_vars()
    {
        string src_file_name = get_file_name_to_write();
        string dest_file_name = get_dir_name_to_write()+"/save_vars_test";
        remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
        remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

        DatABELBaseCPP *fv = new filevector ( src_file_name, 64 );
        unsigned long int obs_indexes[2] =  {1,3};
        fv->save_vars( dest_file_name, 2, obs_indexes );

        DatABELBaseCPP *fv_copy = new filevector( dest_file_name, 2 );
        CPPUNIT_ASSERT_EQUAL(fv->get_nobservations(),fv_copy->get_nobservations());
        CPPUNIT_ASSERT_EQUAL( (unsigned int )2 ,fv_copy->get_nvariables() );
    }

    void test_save_obs()
    {
        string src_file_name = get_file_name_to_write();
        string dest_file_name = get_dir_name_to_write()+"/save_obs_test";
        remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
        remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

        DatABELBaseCPP *fv = new filevector( src_file_name, 64 );
        unsigned int orig_nvars = fv->get_nvariables();
        float * orig_var = new float [fv->get_nobservations()];
        fv->read_variable_as(1, orig_var);

        unsigned long int obs_indexes[2] =  {1,3};
        fv->save_obs( dest_file_name, 2, obs_indexes );
        delete fv;

        DatABELBaseCPP* fv_copy = new filevector( dest_file_name, 2 );
        CPPUNIT_ASSERT_EQUAL((unsigned int )2 ,fv_copy->get_nobservations());
        CPPUNIT_ASSERT_EQUAL( orig_nvars ,fv_copy->get_nvariables() );
        float * saved_var = new float [fv_copy->get_nobservations()];
        fv_copy->read_variable_as(1, saved_var);
        CPPUNIT_ASSERT_EQUAL( orig_var[1] ,saved_var[0] );
        CPPUNIT_ASSERT_EQUAL( orig_var[3] ,saved_var[1] );



        delete fv_copy;
        delete orig_var;
        delete saved_var;
    }

    /*test saving vars/obs window*/
	void test_save_vars_obs()
	{
		string src_file_name = get_file_name_to_write();

		string dest_file_name = get_dir_name_to_write()+"/save_test";
		remove((dest_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
		remove((dest_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));

		DatABELBaseCPP* fv =  new filevector ( src_file_name, 2 );//no need in big cache

		unsigned long int var_indexes[3] =  {1,7,18};
		unsigned long int obs_indexes[2] =  {1,3};
		fv->save( dest_file_name , 3, 2, var_indexes,obs_indexes);

		DatABELBaseCPP* fv_copy = new filevector( dest_file_name, 2 );
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong number of variables in fv_copy", (unsigned int )3,fv_copy->get_nvariables());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong number of observations in fv_copy", (unsigned int )2 ,fv_copy->get_nobservations());

        float * orig_var = new float [fv->get_nobservations()];
        float * saved_var = new float [fv_copy->get_nobservations()];

        for(int i = 0 ;i<3 ; i++ )
        {
			fv->read_variable_as(var_indexes[i], orig_var);
			fv_copy->read_variable_as(i, saved_var);
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

    void test_set_cachesizeMb()
    {
        string src_file_name = get_file_name_to_write();
        filevector *fv = new filevector( src_file_name, 64 );

        unsigned long int vars_capacity =11389 ;
        CPPUNIT_ASSERT_EQUAL( vars_capacity ,fv->cache_size_nvars );

        float * var = new float [fv->get_nobservations()];
        fv->read_variable(0,var);
        CPPUNIT_ASSERT_EQUAL((unsigned long int )0 ,fv->in_cache_from );
        CPPUNIT_ASSERT_EQUAL( vars_capacity-1, fv->in_cache_to );


        fv->set_cachesizeMb(1);
        CPPUNIT_ASSERT_EQUAL((unsigned long int )177 ,fv->cache_size_nvars );
        CPPUNIT_ASSERT_EQUAL((unsigned long int )0 ,fv->in_cache_from );
        CPPUNIT_ASSERT_EQUAL((unsigned long int )0 ,fv->in_cache_to );

        fv->read_variable(fv->get_nvariables() - 1,var);

        CPPUNIT_ASSERT_EQUAL((unsigned long int )fv->get_nvariables() - 1,fv->in_cache_from );
        CPPUNIT_ASSERT_EQUAL((unsigned long int )fv->get_nvariables() - 1,fv->in_cache_to );

        delete var;
    }

	void test_read_write_observation()
	{
	    unsigned long nvariables = 5;
		unsigned long nobservations = 3;
        create_empty_filevector(nvariables, nobservations);
        string tmp_file_name = get_temp_file_name();
		DatABELBaseCPP* fv = new filevector( tmp_file_name, 1 );

        float * var1 = new float [fv->get_nvariables()];
        float * var2 = new float [fv->get_nvariables()];
        int i;
        for(i = 0; i<fv->get_nvariables(); i++)
        {
            var1[i] = i;
		}

        fv->write_observation(2, var1);
        fv->read_observation(2, var2);

        CPPUNIT_ASSERT( compare_arrays(var1, var2, fv->get_nobservations()));
        delete fv;

		//reopen file and check
		DatABELBaseCPP* fv2 = new filevector( tmp_file_name, 1 );
		fv2->read_observation(2,var2);
		CPPUNIT_ASSERT( compare_arrays(var1, var2 , nvariables));
        delete fv2;

        delete [] var1;
        delete [] var2;
	}


    void test_read_variable_convert_to()
	{

		unsigned long int nvariables =10;
		unsigned long int nobservations =3;
		create_empty_filevector(nvariables, nobservations);
		string tmp_file_name = get_temp_file_name();

		DatABELBaseCPP* fv = new filevector( tmp_file_name, 1 );

		float * var = new float [fv->get_nobservations()];
		for(int i = 0; i<fv->get_nobservations(); i++)
        {
            var[i] = i + (float)i/10;
		}
		fv->write_variable_as(1,var);

		int * int_var = new int[fv->get_nobservations()];
		fv->read_variable_as(1,int_var);

		CPPUNIT_ASSERT_EQUAL(0, int_var[0]);
		CPPUNIT_ASSERT_EQUAL(1, int_var[1]);
		CPPUNIT_ASSERT_EQUAL(2, int_var[2]);

		delete[] var;
		delete[] int_var;
	}

	void test_add_variable()
	{
		create_empty_filevector(10,20);
		string tempFileName = get_temp_file_name();
		DatABELBaseCPP* fv = new filevector( tempFileName, 1 );
		float * var = new float [fv->get_nobservations()];
		create_and_fill_variable(fv->get_nobservations(),var);

        string varname = "added";
        string varname2 = "added2";
		fv->add_variable_as(var,varname);
		fv->add_variable_as(var,varname2);

		CPPUNIT_ASSERT_EQUAL((unsigned int)12, fv->get_nvariables());
		fixedchar _fc_varname_loaded = fv->read_variable_name(10);
        CPPUNIT_ASSERT_EQUAL( varname, string(_fc_varname_loaded.name) );
        _fc_varname_loaded = fv->read_variable_name(11);
        CPPUNIT_ASSERT_EQUAL( varname2, string(_fc_varname_loaded.name) );
        delete fv;

        tempFileName = get_temp_file_name();
        DatABELBaseCPP* fv2 = new filevector( tempFileName, 1 );

        CPPUNIT_ASSERT_EQUAL((unsigned int)12, fv2->get_nvariables());
		_fc_varname_loaded = fv2->read_variable_name(10);
        CPPUNIT_ASSERT_EQUAL( varname, string(_fc_varname_loaded.name) );
        _fc_varname_loaded = fv2->read_variable_name(11);
        CPPUNIT_ASSERT_EQUAL( varname2, string(_fc_varname_loaded.name) );
        delete fv2;
	}

	void test_extract_base_file_name()
	{
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

	//==========================  utility methods ==================

    void create_and_fill_variable(unsigned int  nobs, float * in )
    {
        for(int i = 0; i<nobs; i++)
        {
            in[i] = i + (float)i/10;
		}
    }

	void create_empty_filevector(unsigned int  nvars, unsigned int  nobs)
	{
	    string tmp_file_name = get_temp_file_name();
		remove((tmp_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
		remove((tmp_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));
		initialize_empty_file( (char *)tmp_file_name.c_str(), nvars, nobs, FLOAT);
	}

	bool compare_arrays(float * a1,float * a2, int size)
	{
		for(int i =0; i< size ; i++)
		{
			if(a1[i] != a2[i])
			{
			  cout<< "compare_arrays: " << i<<" elements not equal:"<< a1[i]<<","<<a2[i]<<endl;
			  return false;
			}
		}
		return true;
	}


};



#endif


