#ifndef __TESTUTIL__
#define __TESTUTIL__


#include <string>
#include <cmath>
#include <time.h>
#include <stdlib.h>

using namespace std;

/*
* This class provides utility methods to be used by tests
*/
class TestUtil
{
private :
    static string base_dir;

public :
    /* detects executed binary file dir to use it as a base dir to find data files etc */
    static void detect_base_dir(string binpath);
    static string get_base_dir();
	static string get_dir_name_to_write();
	static string get_temp_file_name();


    static float random_float();

    static void initRandomGenerator();

    // returns relative difference between two values
    template<class DT> static DT relativeDifference(DT x, DT y) {
        return abs(2.*(abs(x)-abs(y))/(abs(x)+abs(y)));
    }

    static void create_and_fill_variable(unsigned int  nobs, float * in );
    static void create_empty_filevector(string tmp_file_name,unsigned long nvars, unsigned long nobs);
    static void create_empty_filevector(string tmp_file_name,unsigned long nvars, unsigned long nobs, unsigned short int type);
    static bool compare_arrays(float * a1,float * a2, int size);
};

#endif
