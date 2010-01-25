#include "filevector.h"


class transpose
{
public :
void write_var_obs_names(filevector * src_fv, filevector * dest_fv);

void copy_data(string src_data_file_name,string dest_data_file_name, unsigned long int src_nvars,
unsigned long int src_nobss, unsigned int data_size);
};
