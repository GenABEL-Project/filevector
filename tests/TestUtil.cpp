#include <string>
#include <iostream>

#include "frutil.h"
#include "frerror.h"
#include "filevector.h"
#include "TestUtil.h"

using namespace std;

string TestUtil::base_dir;

void TestUtil::detect_base_dir(string binpath)
{
    //cout << "binary path:" << binpath <<endl;
    size_t slashpos;
    slashpos =  binpath.find_last_of("/");
    //cout << "slashpos:" << slashpos << endl;
    if (slashpos == string::npos)
        error("cannot find slash in path to binary file");

    string basedir = binpath.substr(0,slashpos);
    //cout << "basedir :" << basedir << endl;

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


void TestUtil::create_and_fill_variable(unsigned int  nobs, float * in )
{
	for(int i = 0; i<nobs; i++)
	{
		in[i] = i + (float)i/10;
	}
}

void TestUtil::create_empty_filevector(string tmp_file_name ,unsigned int  nvars, unsigned int  nobs)
{
    create_empty_filevector(tmp_file_name ,nvars, nobs, FLOAT);
}
void TestUtil::create_empty_filevector(string tmp_file_name ,unsigned int  nvars, unsigned int  nobs, unsigned short int type)
{
	remove((tmp_file_name+FILEVECTOR_DATA_FILE_SUFFIX).c_str( ));
	remove((tmp_file_name+FILEVECTOR_INDEX_FILE_SUFFIX).c_str( ));
	initialize_empty_file( (char *)tmp_file_name.c_str(), nvars, nobs, type);
}

bool TestUtil::compare_arrays(float * a1,float * a2, int size)
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

string TestUtil::get_temp_file_name()
{
    return TestUtil::get_base_dir() + string("/../tests/data/2write/tmp");
}

string TestUtil::get_dir_name_to_write()
{
    return TestUtil::get_base_dir() + string("/../tests/data/2write");
}



