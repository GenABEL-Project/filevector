#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "FileModificationTest.h"
#include "TestUtil.h"
#include "FVUnitTest.h"

using namespace std;

string FVUnitTest::get_file_name_to_write()
{
    return TestUtil::get_base_dir() + string("/../tests/data/2write/modify_me.fvi");
}

string FVUnitTest::get_temp_file_name()
{
    return TestUtil::get_base_dir() + string("/../tests/data/2write/tmp");
}

string FVUnitTest::get_dir_name_to_write()
{
    return TestUtil::get_base_dir() + string("/../tests/data/2write");
}


int main( int argc, char **argv)
{
    TestUtil::detect_base_dir(string(argv[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( FVUnitTest::suite() );
    runner.run();
    return 0;
}


