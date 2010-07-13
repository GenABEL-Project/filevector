#include <iostream>
#include <cstdlib>
#include <cstring>

#include "../fvlib/FileVector.h"

int main ( int argc, char * argv[] )
{
	if ( argc<2 )
	{
		testDbg << "Please provide filename!"<< "\n";
		exit ( 1 );
	}
    char * ifname1 = argv[1];

	unsigned long cachesize = 80;
	if (argc>2) cachesize = atoi(argv[2]);
	if (cachesize < 0) {
	    errorLog <<"cache size must be positive long integer\n\n" << errorExit;
	}

	AbstractMatrix *indata1 = new FileVector( ifname1, cachesize );

	testDbg << "Reading file:"<< ifname1 << "\n";
	testDbg << "Using cache size:"<< cachesize<< "\n";
	testDbg << "Variables:"<< indata1->getNumVariables() << "\n";
	testDbg << "Observations:"<< indata1->getNumObservations() << "\n";

	float tmpdat ;
/*	if(!tmpdat )
	{
		testDbg << "Cannot allocate buffer" << "\n";
		exit(1);
	}*/

/*        for ( unsigned long i=0 ; i < indata1->getNumVariables() ; i++ )
        {
            indata1->readVariableAs( i, tmpdat );
            if( i % 100 == 0 )
            {
                testDbg << "Read:"<< i << "/" << indata1->getNumVariables() << " variables \n";
            }
        }

        for ( long i=indata1->getNumVariables() -1; i >=0  ; i-- )
        {
            indata1->readVariableAs( i, tmpdat );
            if( i % 100 == 0 )
            {
                testDbg << "Read:"<< i << "/" << indata1->getNumVariables() << " variables \n";
            }
        }*/

       for ( unsigned long j= 0; j <indata1->getNumObservations() ; j++ )
       for ( unsigned long i= 0; i <indata1->getNumVariables() ; i++ )
        {
            indata1->readElementAs( i,j, tmpdat );
        }

    delete indata1;

	testDbg << "Finished";
}
