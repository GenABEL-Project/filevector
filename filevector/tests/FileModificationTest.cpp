#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "FileModificationTest.h"
#include "TestUtil.h"
#include "../fvlib/Logger.h"

using namespace std;

string FileModificationTest::getFilenameToWrite()
{
    return TestUtil::get_base_dir() + string("/../tests/data/2write/modify_me");
}

int main( int argc, char **argv)
{
    TestUtil::detect_base_dir(string(argv[0]));
    testDbg << "base_dir :" << TestUtil::get_base_dir() << endl;

    CppUnit::TextUi::TestRunner runner;
    runner.addTest( FileModificationTest::suite() );
    runner.run();
    return 0;
}
