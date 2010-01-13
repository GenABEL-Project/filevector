/*
* This is utility to convert old format fvf files: split into data and metadata files.
*/

#include <iostream>
#include <cstdlib>
#include <cstring>

#include "frerror.h"
#include "frutil.h"
#include "filevector.h"

int main(int argc, char * argv[])
{
	if ( argc<2 )
  	{
  		std::cout << "Please provide name of the file to convert"<< endl;
  		exit ( 1 );
  	}
    string filename = argv[1];
    if(filename.find(".fvf")!=filename.length()-4)
       error("filename should have .fvf extention");

    string data_filename =filename.substr(0,filename.length()-4)+ ".fvd";
    string index_filename =filename.substr(0,filename.length()-4)+ ".fvi";

    cout << "data:" << data_filename << ", index:" << index_filename << endl;

    fr_type data_type;
	struct stat filestatus;

	stat( filename.c_str() , &filestatus);


	if (filestatus.st_size < sizeof(data_type))
		error("file %s is too short to contain an FVF-object\n",filename.c_str());

	std::fstream data_file;
	std::ofstream new_data_file;
	std::ofstream new_index_file;

	data_file.open(filename.c_str(), std::ios::out | std::ios::in | std::ios::binary);
	if (data_file.fail())
		error("opening file %s for write & read failed",filename.c_str());

	data_file.read((char*)&data_type,sizeof(data_type));
	if (data_file.fail())
		error("failed to read datainfo from file '%s'",filename.c_str());

	unsigned long int header_size = sizeof(data_type) + sizeof(fixedchar)*(data_type.nvariables+data_type.nobservations);

    new_data_file.open(data_filename.c_str(), std::ios::out | std::ios::binary);
    	if (new_data_file.fail())
    		error("creating new data file %s for write failed",data_filename.c_str());

    new_index_file.open(index_filename.c_str(), std::ios::out | std::ios::binary);
        	if (new_index_file.fail())
        		error("creating new index file %s for write failed",index_filename.c_str());



     //copy header
     int buf_len = header_size;
     char * buffer = new (std::nothrow) char [buf_len];
     if (!buffer)
		error("failed to get memory for buffer\n");
		
     data_file.seekg( 0 );
     data_file.read(buffer,buf_len);
     new_index_file.write(buffer,buf_len);

     cout<< "index copied, copying data..."<<endl;

     //copy data
     while(!data_file.eof())
     {
       data_file.read(buffer,buf_len);
       new_data_file.write(buffer,data_file.gcount());
     }

	 cout << "DONE" << endl;

     data_file.close();
     new_data_file.close();
     new_index_file.close();
     delete buffer;

}