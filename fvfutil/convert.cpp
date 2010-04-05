/*
* This is utility to convert old format fvf files: split into data and metadata files.
*/

#include <iostream>
#include <cstdlib>
#include <cstring>

#include "../fvlib/frerror.h"
#include "../fvlib/frutil.h"
#include "../fvlib/FileVector.h"

using namespace std;

int main(int argc, char * argv[])
{
	if ( argc<2 )
  	{
  		cout << "Please provide name of the file to convert"<< endl;
  		exit ( 1 );
  	}
    string filename = argv[1];
    if(filename.find(".fvf")!=filename.length()-4)
       errorLog << "filename should have .fvf extention" << errorExit;

    string dataFilename = filename.substr(0,filename.length()-4) + FILEVECTOR_DATA_FILE_SUFFIX;
    string indexFilename = filename.substr(0,filename.length()-4) + FILEVECTOR_INDEX_FILE_SUFFIX;

    dbg << "data:" << dataFilename << ", index:" << indexFilename << endl;

    FileHeader data_type;
	struct stat filestatus;

	stat( filename.c_str() , &filestatus);


	if (filestatus.st_size < sizeof(data_type))
		errorLog << "File " << filename <<" is too short to contain an FVF-object." << endl << errorExit;

	fstream dataFile;
	ofstream new_data_file;
	ofstream new_index_file;

	dataFile.open(filename.c_str(), ios::out | ios::in | ios::binary);
	if (dataFile.fail()) {
		errorLog << "Opening file "<< filename << "for write & read failed" << errorExit;
	}

	dataFile.read((char*)&data_type,sizeof(data_type));
	if (dataFile.fail()) {
		errorLog << "Failed to read datainfo from file " << filename << errorExit;
	}

	unsigned long headerSize = sizeof(data_type) + sizeof(FixedChar)*(data_type.numVariables+data_type.numObservations);

    new_data_file.open(dataFilename.c_str(), ios::out | ios::binary);
    if (new_data_file.fail())
    	errorLog << "Creating new data file " << dataFilename << " for write failed" << endl << errorExit;

    new_index_file.open(indexFilename.c_str(), ios::out | ios::binary);
    if (new_index_file.fail())
    	errorLog << "Creating new index file "<<indexFilename<<" for write failed"<< endl<< errorExit;

     //copy header
    int buf_len = headerSize;
    char * buffer = new (nothrow) char [buf_len];
    if (!buffer)
		errorLog << "failed to get memory for buffer " << endl << errorExit;
		
     dataFile.seekg( 0 );
     dataFile.read(buffer,buf_len);
     new_index_file.write(buffer,buf_len);

     dbg << "index copied, copying data..." << endl;

     //copy data
     while(!dataFile.eof())
     {
       dataFile.read(buffer,buf_len);
       new_data_file.write(buffer,dataFile.gcount());
     }

	 dbg << "DONE" << endl;

     dataFile.close();
     new_data_file.close();
     new_index_file.close();
     delete buffer;

}