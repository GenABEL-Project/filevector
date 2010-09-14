#include "TestUtil.h"
#include "ReusableFileHandle.h"
#include "ReusableFileHandleTest.h"

using namespace std;

void ReusableFileHandleTest::testRandAccess() {
    string fileName = TestUtil::get_dir_name_to_write() + string("/some.bin");

    ofstream fcreator(fileName.c_str(), ios::binary | ios::out);
    fcreator.close();
    
    ReusableFileHandle rfh1 = ReusableFileHandle::getHandle(fileName, false);
    ReusableFileHandle rfh2 = ReusableFileHandle::getHandle(fileName, false);

    char x = 'X';

    rfh1.fseek(2);
    rfh1.blockWriteOrRead(1, &x, true);
    rfh1.fseek(12);
    rfh1.blockWriteOrRead(1, &x, true);

    char y = 0;
    rfh2.fseek(2);
    rfh2.blockWriteOrRead(1, &y, false);
    CPPUNIT_ASSERT_EQUAL('X', y);

    y=0;

    rfh2.fseek(12);
    rfh2.blockWriteOrRead(1, &y, false);

    CPPUNIT_ASSERT_EQUAL('X', y);

    rfh2.close();

    y=0;
    rfh1.fseek(2);
    rfh1.blockWriteOrRead(1, &y, false);
    CPPUNIT_ASSERT_EQUAL('X', y);

    rfh1.close();

    CPPUNIT_ASSERT_EQUAL(ReusableFileHandle::testGetNumHandles(), 0);
}

int main( int argc, char **argv)
{
    TestUtil::detect_base_dir(string(argv[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( ReusableFileHandleTest::suite() );
    runner.run();
    return 0;
}