#include <string>

using namespace std;

class ConvertTest : public CppUnit::TestFixture {
    void test();

    CPPUNIT_TEST_SUITE( ConvertTest );
    CPPUNIT_TEST( test );
    CPPUNIT_TEST_SUITE_END();

public:
    ConvertTest( ) : CppUnit::TestFixture(  ) {}
};
