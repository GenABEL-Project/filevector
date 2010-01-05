#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#include <frvector.h>

using namespace std;


class FileModificationTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( FileModificationTest );
    CPPUNIT_TEST( testRandomWrite );
    CPPUNIT_TEST_SUITE_END();

public:

    static string base_dir;
    string detect_base_dir(string binpath);

    float random_float()
    {
        float scale=RAND_MAX+1.;
        float base=rand()/scale;
        float fine=rand()/scale;
        return base+fine/scale;
    };

    /* make full path to file being written */
    string get_file_name_to_write();

    /*
     * TODO Spec is not clear here, need to clarify what to test and how
     */
    void testRandomWrite()
    {
        string file_name = get_file_name_to_write();
        cout<< "full path:" << get_file_name_to_write() << endl;
        filevector<float> fv( file_name, 64 );
        std::cout << "fv.get_nvariables():" << fv.get_nvariables() << "\n";
        CPPUNIT_ASSERT(fv.get_nvariables() == 33815 );

        int var_write_count = fv.get_nvariables() / 1000;
        int el_write_count = fv.get_nobservations() / 10;

        cout << "writing random data to variables\n";
        for (int i=0;i<var_write_count;i++)
        {
            int var_num = (rand()%fv.get_nvariables())+1; //random number of variable in the file
            float * tmpdat = new (std::nothrow) float [fv.get_nobservations()];
            fv.write_variable( var_num,tmpdat);
            delete tmpdat;
        }
        cout << "wrote "<< var_write_count <<" variables\n";

        cout << "writing random data using write_element()\n";
        for (int i=0;i<el_write_count;i++)
        {
            int var_num = (rand()%fv.get_nvariables())+1;
            int obs_num = (rand()%fv.get_nobservations())+1;
            float tmp_elem = random_float();
            fv.write_element( var_num, obs_num, tmp_elem);
        }
        cout << "wrote "<< el_write_count <<" elements\n";

        cout << "done\n";
    }

    void testWriteBigFile()
    {

    }
};

