#include "TestUtil.h"
#include "TransposeTest.h"

using namespace std;


int main( int argc, char **argv)
{
    TestUtil::detect_base_dir(string(argv[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( TransposeTest::suite() );
    runner.run();
    return 0;
}