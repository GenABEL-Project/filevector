#ifndef ___TRANSPOSE___
#define ___TRANSPOSE___

#include "filevector.h"


class transpose
{
public :
void process(string filename);

void write_var_obs_names(filevector * src_fv, filevector * dest_fv);

void copy_data(string src_data_file_name,string dest_data_file_name, unsigned long int src_nvars,
unsigned long int src_nobss, unsigned int data_size);

void read_part(ifstream * src_stream, char * data_part, unsigned long int obs_start , unsigned long int obs_length,
unsigned long int var_start, unsigned long int var_length , unsigned int  data_size);

void write_part(ofstream * src_stream, char * data_part_transposed, unsigned long int obs_start , unsigned long int obs_length,
unsigned long int var_start, unsigned long int var_length , unsigned int  data_size);

void transpose_part(void * data_part, void * data_part_transposed,
unsigned long int obs_length,unsigned long int var_length, unsigned int data_size );

};

#endif