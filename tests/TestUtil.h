#ifndef __TESTUTIL__
#define __TESTUTIL__


#include <string>

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

    static float random_float();

};

#endif
