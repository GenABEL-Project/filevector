#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#include <frvector.h>

class FileModificationTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( FileModificationTest );
    CPPUNIT_TEST( testRandomWrite );
    CPPUNIT_TEST_SUITE_END();

public:

    void testRandomWrite()
    {
        char * file_name = (char * )"/home/chernyh/work/projects/cpp/data/2write/modify_me.fvf";
        filevector<float> fv( file_name, 64 );
        printf( "fv.get_nvariables(): %lu \n",fv.get_nvariables());
        CPPUNIT_ASSERT(fv.get_nvariables() == 33815 );
    }
};


int main( int argc, char **argv)
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( FileModificationTest::suite() );
    runner.run();
    return 0;
}
