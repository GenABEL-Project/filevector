#include <string>

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include "ConvertTest.h"
#include "TestUtil.h"
#include "UnitTest.h"
#include "FileVector.h"
#include "frutil.h"
#include "AbstractMatrix.h"

void ConvertTest::test() {
    unsigned long numVariables = 3;
    unsigned long nobservations = 3;
    string tmp_file_name = TestUtil::get_base_dir() + string("/../tests/data/2write/txttest");
    string textFileName = TestUtil::get_base_dir() + string("/../tests/data/2write/test.txt");

  //string filename, unsigned long numVariables, unsigned long numObservations, unsigned short int type, bool override
    initializeEmptyFile(tmp_file_name, numVariables, nobservations, FLOAT, true);
    AbstractMatrix* fv = new FileVector( tmp_file_name, 1);

    float * var = new float [fv->getNumObservations()];
	
    for(int i = 0; i< fv->getNumObservations(); i++) {
        var[i] = i + 1.1;
    }

    fv->writeVariableAs(0, var);
    fv->writeVariableAs(1, var);
    fv->writeVariableAs(2, var);

    delete[] var;

    fv->saveAsText(textFileName, true, true);

    delete fv;

    ifstream txt(textFileName.c_str());
    string s;float f;


    txt >> s; CPPUNIT_ASSERT_EQUAL(string("1"), s);
    txt >> s; CPPUNIT_ASSERT_EQUAL(string("2"), s);
    txt >> s; CPPUNIT_ASSERT_EQUAL(string("3"), s);

    txt >> s; CPPUNIT_ASSERT_EQUAL(string("1"), s);

    txt >> s; parseStringToArbType(s,FLOAT,&f,"NAN");CPPUNIT_ASSERT(TestUtil::relativeDifference(1.1F,f)<.001);
    txt >> s; parseStringToArbType(s,FLOAT,&f,"NAN");CPPUNIT_ASSERT(TestUtil::relativeDifference(2.1F,f)<.001);
    txt >> s; parseStringToArbType(s,FLOAT,&f,"NAN");CPPUNIT_ASSERT(TestUtil::relativeDifference(3.1F,f)<.001);

    txt >> s; CPPUNIT_ASSERT_EQUAL(string("2"), s);
    txt >> s; parseStringToArbType(s,FLOAT,&f,"NAN");CPPUNIT_ASSERT(TestUtil::relativeDifference(1.1F,f)<.001);
    txt >> s; parseStringToArbType(s,FLOAT,&f,"NAN");CPPUNIT_ASSERT(TestUtil::relativeDifference(2.1F,f)<.001);
    txt >> s; parseStringToArbType(s,FLOAT,&f,"NAN");CPPUNIT_ASSERT(TestUtil::relativeDifference(3.1F,f)<.001);

    txt >> s; CPPUNIT_ASSERT_EQUAL(string("3"), s);
    txt >> s; parseStringToArbType(s,FLOAT,&f,"NAN");CPPUNIT_ASSERT(TestUtil::relativeDifference(1.1F,f)<.001);
    txt >> s; parseStringToArbType(s,FLOAT,&f,"NAN");CPPUNIT_ASSERT(TestUtil::relativeDifference(2.1F,f)<.001);
    txt >> s; parseStringToArbType(s,FLOAT,&f,"NAN");CPPUNIT_ASSERT(TestUtil::relativeDifference(3.1F,f)<.001);
}

int main(int numargs, char**args) {
    TestUtil::detect_base_dir(string(args[0]));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( ConvertTest::suite() );
    runner.run();
    return 0;
}