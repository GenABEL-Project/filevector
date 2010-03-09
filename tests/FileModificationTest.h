#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#include "filevector.h"
#include "TestUtil.h"

using namespace std;


class FileModificationTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( FileModificationTest );
    CPPUNIT_TEST( testRandomWrite );
    CPPUNIT_TEST_SUITE_END();

public:
    /* make full path to file being written */
    string get_file_name_to_write();

    /*
     * TODO Spec is not clear here, need to clarify what to test and how
     */
    void testRandomWrite()
    {
        string file_name = get_file_name_to_write();
        dbg << "full path:" << get_file_name_to_write() << nl;
        AbstractMatrix* fv = new filevector ( file_name, 64 );
        dbg << "fv.getNumVariables():" << fv->getNumVariables() << "\n";
        CPPUNIT_ASSERT(fv->getNumVariables() == 33815 );

        int var_write_count = fv->getNumVariables() / 1000;
        int el_write_count = fv->getNumObservations() / 10;

        dbg << "writing random data to variables\n";
        for (int i=0;i<var_write_count;i++)
        {
            int var_num = (rand()%fv->getNumVariables())+1; //random number of variable in the file
            float * tmpdat = new (std::nothrow) float [fv->getNumObservations()];
            fv->writeVariableAs( var_num, tmpdat);
            delete tmpdat;
        }
        dbg << "wrote "<< var_write_count <<" variables\n";

        dbg << "writing random data using writeElement()\n";
        for (int i=0;i<el_write_count;i++)
        {
            int var_num = (rand()%fv->getNumVariables())+1;
            int obs_num = (rand()%fv->getNumObservations())+1;
            float tmp_elem = TestUtil::random_float();
            fv->writeElementAs( var_num, obs_num, tmp_elem);
        }
        dbg << "wrote "<< el_write_count <<" elements\n";

        dbg << "done\n";
        delete fv;
    }

    void testWriteBigFile()
    {

    }
};

