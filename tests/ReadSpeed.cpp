#include <iostream>
#include <cstdlib>
#include <cstring>

#include "filevector.h"

int main ( int argc, char * argv[] )
{
	if ( argc<2 )
	{
		dbg << "Please provide filename!"<< "\n";
		exit ( 1 );
	}
    char * ifname1 = argv[1];

	unsigned long int cachesize = 64;
	if (argc>2) cachesize = atoi(argv[2]);
	if (cachesize < 0) error ("cache size must be positive long integer\n\n");

	AbstractMatrix *indata1 = new filevector( ifname1, cachesize );

	dbg << "Reading file:"<< ifname1 << "\n";
	dbg << "Using cache size:"<< cachesize<< "\n";
	dbg << "Variables:"<< indata1->getNumVariables() << "\n";
	dbg << "Observations:"<< indata1->getNumObservations() << "\n";

	float * tmpdat = new ( std::nothrow ) float [indata1->getNumObservations()];
	if(!tmpdat )
	{
		dbg << "Cannot allocate buffer" << "\n";
		exit(1);
	}

        for ( unsigned long int i=0 ; i < indata1->getNumVariables() ; i++ )
        {
            indata1->readVariableAs( i, tmpdat );
            if( i % 10000 == 0 )
            {
                dbg << "Read:"<< i << "/" << indata1->getNumVariables() << " variables \n";
            }
        }

	delete tmpdat;
        delete indata1;

	dbg << "Finished";
}
