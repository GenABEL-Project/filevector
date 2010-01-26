/*
* This is utility to transpose filevector files in binary format, so there will not be need in
advanced text tools behaviour.
*/

#include <iostream>
#include <cstdlib>
#include <cstring>

#include "frerror.h"
#include "frutil.h"
#include "filevector.h"
#include "transpose.h"

using namespace std;

int main(int argc, char * argv[])
{
	if ( argc<2 )
  	{
  		cout << "Please provide name of the file to transpose"<< endl;
  		exit ( 1 );
  	}
    string filename = argv[1];

    transpose tr;
    tr.process(filename);
}

