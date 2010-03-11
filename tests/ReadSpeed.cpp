#include <iostream>
#include <cstdlib>
#include <cstring>

#include "../fvlib/filevector.h"

int main ( int argc, char * argv[] )
{
	if ( argc<2 )
	{
		testDbg << "Please provide filename!"<< "\n";
		exit ( 1 );
	}
    char * ifname1 = argv[1];

	unsigned long int cachesize = 64;
	if (argc>2) cachesize = atoi(argv[2]);
	if (cachesize < 0) {
	    err <<"cache size must be positive long integer\n\n" << errorExit;
	}

	AbstractMatrix *indata1 = new filevector( ifname1, cachesize );

	testDbg << "Reading file:"<< ifname1 << "\n";
	testDbg << "Using cache size:"<< cachesize<< "\n";
	testDbg << "Variables:"<< indata1->getNumVariables() << "\n";
	testDbg << "Observations:"<< indata1->getNumObservations() << "\n";

	float * tmpdat = new ( nothrow ) float [indata1->getNumObservations()];
	if(!tmpdat )
	{
		testDbg << "Cannot allocate buffer" << "\n";
		exit(1);
	}

        for ( unsigned long int i=0 ; i < indata1->getNumVariables() ; i++ )
        {
            indata1->readVariableAs( i, tmpdat );
            if( i % 10000 == 0 )
            {
                testDbg << "Read:"<< i << "/" << indata1->getNumVariables() << " variables \n";
            }
        }

	delete tmpdat;
        delete indata1;

	testDbg << "Finished";
}
