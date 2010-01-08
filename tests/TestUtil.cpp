#include <string>

#include "frerror.h"
#include "TestUtil.h"

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


