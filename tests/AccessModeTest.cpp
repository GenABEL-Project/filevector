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

void AccessModeTest::testReadOnly() {
    string fileName = getDirName() + string("/db_create");
    DatABELBaseCPP * f = new filevector(fileName, 1);
    int p = 0;
    f->write_element_as(1,1,p);
    delete f;
}

int main(int numArgs, char **args){
    TestUtil::detect_base_dir(string(args[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( AccessModeTest::suite() );
    runner.run();
    return 0;
}
