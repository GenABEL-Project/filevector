#include <iostream>
#include <fstream>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#include "AccessModeTest.h"
#include "TestUtil.h"

#include "../fvlib/FileVector.h"

using namespace std;

string getDirName() {
    return TestUtil::get_base_dir() + string("/../tests/data/modetests");
}

void AccessModeTest::testReadOnly() {
    string fileName = getDirName() + string("/db_create");
    initializeEmptyFile(fileName, 3,5, DOUBLE, true );

    AbstractMatrix * f = new FileVector(fileName, 1);
    int p = 0;
    f->writeElementAs(1, 1, p);

    delete f;
}

int main(int numArgs, char **args){
    TestUtil::detect_base_dir(string(args[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( AccessModeTest::suite() );
    runner.run();
    return 0;
}
