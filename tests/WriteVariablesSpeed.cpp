/* Generate 2,500,000 variables of length 10,000 (random), assign names (vars: rs1-rs2500000, obs: id1-id10000),
Use initialize_empty_file()/write_variable() for 2,000,000 and top up with add_variable() to 2,500,000.
Select 2,500 random variables,  */
#include <sstream>

#include "frerror.h"
#include "frutil.h"
#include "filevector.h"

int main(int argc, char * argv[])
{

	int nvars = 100000;
	int nobs = 1000;
	int to_add = 20000;
	unsigned int cache_size = 64;
	string filename="./writespeed_result";

	printf("Usage: %s nvars(%d) nobs(%d) to_add(%d) cache_size(%d) filename(%s)\n",argv[0],nvars,nobs,to_add,cache_size, filename.c_str());

	if ( argc>1 )
  	    nvars = atoi(argv[1]);
  	if ( argc>2 )
  	    nobs = atoi(argv[2]);
  	if ( argc>3 )
  	    to_add = atoi(argv[3]);
  	if ( argc>4 )
  	    cache_size = atoi(argv[4]);
  	if ( argc>5 )
        filename = string(argv[5]);


  	cout << "Generating data to file "<< filename<< ",vars*obs: "<< nvars<< "*"<<nobs <<" , cache:"<< cache_size << endl;
	initialize_empty_file( (char *)filename.c_str(), nvars, nobs, FLOAT, true);
	filevector fv(filename,cache_size);

    float * tmp = new float[fv.get_nobservations()];
	for(int i=0;i<fv.get_nvariables();i++)
	{
	    fv.write_variable(i, tmp);
	    fv.write_variable_name(i,fixedchar("original"));
	    if( i % 10000 == 0 )
		{
			std::cout << "Wrote:"<< i << "/" << fv.get_nvariables() << " variables " << endl;
		}
	}


	for(int i=0 ; i < to_add ;i++)
	{
		stringstream ss (stringstream::in | stringstream::out);
		ss << "added"<<i;

	    fv.add_variable( tmp, ss.str() );
	    if( i % 10000 == 0 )
		{
			std::cout << "Added:"<< i << "/" << to_add << " variables " << endl;
		}
	}


	delete[] tmp;
}