#include "FileModificationTest.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

string FileModificationTest::base_dir ;

string FileModificationTest::get_file_name_to_write()
{
    return  base_dir + string("/../tests/data/2write/modify_me.fvf");
}

string detect_base_dir(string binpath)
{
    //cout << "binary path:" << binpath <<endl;
    size_t slashpos;
    slashpos =  binpath.find_last_of("/");
    //cout << "slashpos:" << slashpos << endl;
    if (slashpos == string::npos)
        error("cannot find slash in path to binary file");

    string basedir = binpath.substr(0,slashpos);
    //cout << "basedir :" << basedir << endl;
    return basedir;
}

int main( int argc, char **argv)
{

    FileModificationTest::base_dir = detect_base_dir(string(argv[0]));
    cout << "base_dir :" << FileModificationTest::base_dir << "\n";

    CppUnit::TextUi::TestRunner runner;
    runner.addTest( FileModificationTest::suite() );
    runner.run();
    return 0;
}
