#include <iostream>
#include <cstdlib>
#include <cstring>

#include "frvector.h"

int main ( int argc, char * argv[] )
{
	if ( argc<2 )
	{
		std::cout << "Please provide filename!"<< "\n";
		exit ( 1 );
	}
	unsigned long int cachesize = 64;
	char * ifname1 = argv[1];
	filevector<float> indata1( ifname1, cachesize );

	std::cout << "Reading file:"<< ifname1 << "\n";
	std::cout << "Variables:"<< indata1.get_nvariables() << "\n";
	std::cout << "Observations:"<< indata1.get_nobservations() << "\n";

	float * tmpdat = new ( std::nothrow ) float [indata1.get_nobservations()];
	if(!tmpdat )
	{
		std::cout << "Cannot allocate buffer" << "\n";
		exit(1);
	}

	for ( unsigned long int i=0 ; i < indata1.get_nvariables() ; i++ )
	{
		indata1.read_variable( i,tmpdat );
		if( i % 10000 == 0 )
		{
			std::cout << "Read:"<< i << " variables \n";
		}
	}

	delete tmpdat;
	std::cout << "Finished";
}