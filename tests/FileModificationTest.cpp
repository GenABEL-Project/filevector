#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "FileModificationTest.h"
#include "TestUtil.h"

using namespace std;

string FileModificationTest::get_file_name_to_write()
{
    return TestUtil::get_base_dir() + string("/../tests/data/2write/modify_me.fvf");
}

int main( int argc, char **argv)
{
    string file_name = get_file_name_to_write();

    TestUtil::detect_base_dir(string(argv[0]));
    filevector<float> indata1( ifname1, cachesize );

    cout << "base_dir:" << TestUtil::get_base_dir() << endl;

    CppUnit::TextUi::TestRunner runner;
    runner.addTest( FileModificationTest::suite() );
    runner.run();
    return 0;
}
