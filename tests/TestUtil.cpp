#include <string>
#include <iostream>

#include "../fvlib/frutil.h"
#include "../fvlib/frerror.h"
#include "../fvlib/filevector.h"
#include "TestUtil.h"

using namespace std;

string TestUtil::base_dir;

void TestUtil::detect_base_dir(string binpath)
{
    size_t slashpos;
    slashpos = binpath.find_last_of("/");
    if (slashpos == string::npos) {
        errorLog << "cannot find slash in path to binary file" << errorExit;
    }

    string basedir = binpath.substr(0, slashpos);

    TestUtil::base_dir = basedir;
}

string TestUtil::get_base_dir()
{
    return TestUtil::base_dir;
}

void TestUtil::initRandomGenerator(){
    srand (time(0));
}

float TestUtil::random_float()
{
    float scale=RAND_MAX+1.;
    float base=rand()/scale;
    float fine=rand()/scale;
    return base+fine/scale;
}


void TestUtil::create_and_fill_variable(unsigned int  nobs, float * in ) {
	for(int i = 0; i<nobs; i++)
	{
		in[i] = i + (float)i/10;
	}
}

void TestUtil::create_empty_filevector(string tmp_file_name ,unsigned long nvars, unsigned long nobs) {
    create_empty_filevector(tmp_file_name ,nvars, nobs, FLOAT);
}

void TestUtil::create_empty_filevector(string tmp_file_name, unsigned long nvars, unsigned long nobs, unsigned short int type)
{
	remove((tmp_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
	remove((tmp_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));
	initialize_empty_file( (char *)tmp_file_name.c_str(), nvars, nobs, type, true);
}

bool TestUtil::compare_arrays(float * a1, float * a2, int size)
{
	for(int i =0; i< size ; i++)
	{
		if(a1[i] != a2[i])
		{
		  testDbg << "compare_arrays: " << i << " elements not equal:"<< a1[i]<<","<<a2[i]<<endl;
		  return false;
		}
	}
	return true;
}

string TestUtil::get_temp_file_name()
{
    return TestUtil::get_base_dir() + string("/../tests/data/2write/tmp");
}

string TestUtil::get_dir_name_to_write()
{
    return TestUtil::get_base_dir() + string("/../tests/data/2write");
}



