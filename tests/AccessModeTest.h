#include <string>

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

using namespace std;

class AccessModeTest : public CppUnit::TestFixture {
    void test();

    CPPUNIT_TEST_SUITE( AccessModeTest);
    CPPUNIT_TEST( test );
    CPPUNIT_TEST_SUITE_END();

public:
    AccessModeTest( ) : CppUnit::TestFixture(  ) {}
};
