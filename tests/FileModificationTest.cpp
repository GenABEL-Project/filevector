#include "FileModificationTest.h"


int main( int argc, char **argv)
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( FileModificationTest::suite() );
    runner.run();
    return 0;
}
