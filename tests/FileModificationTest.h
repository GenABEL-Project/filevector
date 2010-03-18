#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#include "../fvlib/FileVector.h"
#include "TestUtil.h"

using namespace std;


class FileModificationTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( FileModificationTest );
    CPPUNIT_TEST_SUITE_END();

public:
    /* make full path to file being written */
    string get_file_name_to_write();

};

