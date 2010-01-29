#include <iostream>
#include <fstream>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#include "AccessModeTest.h"
#include "TestUtil.h"

#include "filevector.h"

using namespace std;

string getDirName()
{
    return TestUtil::get_base_dir() + string("/../tests/data/modetests");
}

void AccessModeTest::test() {
/*    string fileName = getDirName() + string("/db_create");
    filevector * f = new filevector(fileName, 1);
    CPPUNIT_ASSERT(file_exists(fileName + FILEVECTOR_DATA_FILE_SUFFIX) == true);
    CPPUNIT_ASSERT(file_exists(fileName + FILEVECTOR_INDEX_FILE_SUFFIX) == true);
    delete f;*/
}


int main(int numArgs, char **args){
    TestUtil::detect_base_dir(string(args[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( AccessModeTest::suite() );
    runner.run();
    return 0;
}
