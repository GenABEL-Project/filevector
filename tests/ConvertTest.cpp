#include <string>

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include "ConvertTest.h"
#include "TestUtil.h"
#include "FVUnitTest.h"
#include "filevector.h"
#include "frutil.h"
#include "DatABELBaseCPP.h"

void ConvertTest::test() {
    unsigned long int nvariables = 3;
    unsigned long int nobservations = 3;
    string tmp_file_name = TestUtil::get_base_dir() + string("/../tests/data/2write/txttest");
    string textFileName = TestUtil::get_base_dir() + string("/../tests/data/2write/test.txt");

  //string filename, unsigned long int nvariables, unsigned long int nobservations, unsigned short int type, bool override
    initialize_empty_file(tmp_file_name, nvariables, nobservations, FLOAT, true);
    DatABELBaseCPP* fv = new filevector( tmp_file_name, 1);

    float * var = new float [fv->get_nobservations()];
	
    for(int i = 0; i< fv->get_nobservations(); i++) {
        var[i] = i + 1.1;
    }

    fv->write_variable_as(0, var);
    fv->write_variable_as(1, var);
    fv->write_variable_as(2, var);

    delete[] var;

    unsigned long varMask[] = {0, 2};
    unsigned long obsMask[] = {1, 2};

    fv->save_as_text(textFileName, 2, 2, varMask, obsMask);

    delete fv;

    ifstream txt(textFileName.c_str());
    string s;
    txt >> s;
    CPPUNIT_ASSERT_EQUAL(string("2"), s);
    txt >> s;
    CPPUNIT_ASSERT_EQUAL(string("3"), s);
    txt >> s;
    CPPUNIT_ASSERT_EQUAL(string("1"), s);
    txt >> s;
    CPPUNIT_ASSERT_EQUAL(string("2.1"), s);
    txt >> s;
    CPPUNIT_ASSERT_EQUAL(string("3.1"), s);
    txt >> s;
    CPPUNIT_ASSERT_EQUAL(string("3"), s);
    txt >> s;
    CPPUNIT_ASSERT_EQUAL(string("2.1"), s);
    txt >> s;
    CPPUNIT_ASSERT_EQUAL(string("3.1"), s);
}

int main(int numargs, char**args) {
    TestUtil::detect_base_dir(string(args[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( ConvertTest::suite() );
    runner.run();
    return 0;
}