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
	dbg << "Variables:"<< indata1->get_nvariables() << "\n";
	dbg << "Observations:"<< indata1->get_nobservations() << "\n";

	float * tmpdat = new ( std::nothrow ) float [indata1->get_nobservations()];
	if(!tmpdat )
	{
		dbg << "Cannot allocate buffer" << "\n";
		exit(1);
	}

        for ( unsigned long int i=0 ; i < indata1->get_nvariables() ; i++ )
        {
            indata1->read_variable_as( i, tmpdat );
            if( i % 10000 == 0 )
            {
                dbg << "Read:"<< i << "/" << indata1->get_nvariables() << " variables \n";
            }
        }

	delete tmpdat;
        delete indata1;

	dbg << "Finished";
}
