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

    filevector* src_fv = new filevector(filename,1);
    unsigned long int src_nvars = src_fv->get_nvariables();
    unsigned long int src_nobss = src_fv->get_nobservations();
    unsigned int data_size = src_fv->getDataSize();

    string dest_file_name = extract_base_file_name(filename)+"_transposed";
    
    string src_data_file_name = extract_base_file_name(filename)+FILEVECTOR_DATA_FILE_SUFFIX;
    string dest_data_file_name = extract_base_file_name(filename)+"_transposed"+FILEVECTOR_DATA_FILE_SUFFIX;
    initialize_empty_file(dest_file_name, src_fv->get_nobservations(), src_fv->get_nvariables(), src_fv->getDataType());

    filevector* dest_fv = new filevector(dest_file_name,1);
    cout<< "Copying var/obs names...";
    transpose tr;
    tr.write_var_obs_names(src_fv, dest_fv);

    delete src_fv;
    delete dest_fv;
    cout<< "done"<< endl;

    tr.copy_data(src_data_file_name,dest_data_file_name,src_nvars,src_nobss,data_size);
    cout<< "done"<< endl;

 }


void transpose::write_var_obs_names(filevector * src_fv, filevector * dest_fv)
{
   // copy observations and variables names
   for( unsigned long int i=0 ; i < src_fv->get_nvariables(); i++ )
     dest_fv->write_observation_name( i, src_fv->read_variable_name(i) );

   for( unsigned long int i=0 ; i < src_fv->get_nobservations(); i++ )
     dest_fv->write_variable_name( i, src_fv->read_observation_name(i) );
}


void transpose::copy_data(string src_data_file_name,string dest_data_file_name, unsigned long int src_nvars,
unsigned long int src_nobss, unsigned int data_size)
{
cout<< "Copying data..."<< src_nobss << "x"<< src_nvars << endl;
  //number of elements in square side. square_size^2 elements will be processed at one time:read and write
  unsigned int square_size = 10000;

  unsigned long int obs_pages = src_nobss / square_size;
  if(src_nobss % square_size > 0) obs_pages++;

  unsigned long int var_pages = src_nvars / square_size;
  if(src_nvars % square_size > 0) var_pages++;

  ifstream * src_stream = new ifstream();
  src_stream->open(src_data_file_name.c_str(),ifstream::in );

  ofstream * dest_stream = new ofstream;
  dest_stream->open(dest_data_file_name.c_str(),ofstream::out );

  for( unsigned long int i =0; i< var_pages;i++)
  {
      for( unsigned long int j =0; j< obs_pages;j++)    
      {
          unsigned long int obs_length = square_size;
          if((j + 1 )* square_size > src_nobss)
              obs_length = src_nobss % square_size; 

          unsigned long int var_length = square_size;
          if((i + 1 )* square_size > src_nvars)
              var_length = src_nvars % square_size;

//          cout << ">"<< obs_length << "x" << var_length << "^ " ;
          cout << ".";
          char * data_part = new (nothrow) char[var_length*obs_length*data_size];
          if(!data_part) error("can not allocate memory for data_part");
          char * data_part_transposed = new (nothrow) char[var_length*obs_length*data_size];
          if(!data_part_transposed) error("can not allocate memory for data_part_transposed");
          
          read_part(src_stream, data_part, j * square_size , obs_length, i * square_size , var_length,  data_size );

          transpose_part(data_part,data_part_transposed,obs_length,var_length, data_size);

          write_part(dest_stream, data_part_transposed, i * square_size, var_length, j* square_size , obs_length,  data_size );

          delete data_part;
          delete data_part_transposed;
      }
      cout << endl;
  }

  src_stream->close();
  delete src_stream;
  dest_stream->close();
  delete dest_stream;

  cout<< "data written" << endl;
}


/*
* read next piece of data with size = obs_length x var_length, starting from var_start, obs_start coordinates    
*/
void transpose::read_part(ifstream * src_stream, char * data_part, unsigned long int obs_start , unsigned long int obs_length,
unsigned long int var_start, unsigned long int var_length , unsigned int  data_size)
{
//cout << "read_part"<<endl;
	for(unsigned long int i=0; i<var_length ;i++)
	{
	   //seek to the beginning of the next var
	   src_stream->seekg( ( var_start + i )* obs_start * data_size );
	   //read next var to input buffer
	   src_stream->read( data_part + ( i * obs_length * data_size ), obs_length * data_size );
	}
}

/*
* write next piece of transposed data with size = obs_length' x var_length'    
*/
void transpose::write_part(ofstream * dest_stream, char * data_part_transposed, unsigned long int obs_start , unsigned long int obs_length,
unsigned long int var_start, unsigned long int var_length , unsigned int  data_size)
{
//cout << "write_part"<<endl;
	for(unsigned long int i=0; i<var_length ;i++)
	{
	   //seek to the beginning of the next var
	   dest_stream->seekp( ( var_start + i )* obs_start * data_size );
	   //read next var to input buffer
	   dest_stream->write( data_part_transposed + ( i * obs_length * data_size ), obs_length * data_size );
	}
}

/*
transpose piece of data to write to the new file.
original axb matrix flipped to bxa matrix.
*/
void transpose::transpose_part(char * data_part, char * data_part_transposed,
unsigned long int obs_length,unsigned long int var_length, unsigned int data_size )
{
//cout << "transpose_part"<<endl;
	for(unsigned long int i=0; i<var_length ;i++)
	{
		for(unsigned long int j=0; j<obs_length ;j++)
		{
		   memcpy(data_part_transposed + j * obs_length* data_size + i * data_size,
		          data_part + i * obs_length* data_size + j * data_size,
		          data_size);
		}
	}
}